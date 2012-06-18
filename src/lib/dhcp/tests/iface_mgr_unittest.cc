// Copyright (C) 2011-2012 Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 // INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <config.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include <asiolink/io_address.h>
#include <dhcp/pkt6.h>
#include <dhcp/iface_mgr.h>
#include <dhcp/dhcp4.h>

using namespace std;
using namespace isc;
using namespace isc::asiolink;
using namespace isc::dhcp;

// name of loopback interface detection
const size_t buf_size = 32;
char LOOPBACK[buf_size] = "lo";

namespace {

class NakedIfaceMgr: public IfaceMgr {
    // "naked" Interface Manager, exposes internal fields
public:
    NakedIfaceMgr() { }
    IfaceCollection & getIfacesLst() { return ifaces_; }
};

// dummy class for now, but this will be expanded when needed
class IfaceMgrTest : public ::testing::Test {
public:
    // these are empty for now, but let's keep them around
    IfaceMgrTest() {
    }

    ~IfaceMgrTest() {
    }
};

// We need some known interface to work reliably. Loopback interface
// is named lo on Linux and lo0 on BSD boxes. We need to find out
// which is available. This is not a real test, but rather a workaround
// that will go away when interface detection is implemented.

// NOTE: At this stage of development, write access to current directory
// during running tests is required.
TEST_F(IfaceMgrTest, loDetect) {

    // poor man's interface detection
    // it will go away as soon as proper interface detection
    // is implemented
    if (if_nametoindex("lo") > 0) {
        cout << "This is Linux, using lo as loopback." << endl;
        snprintf(LOOPBACK, buf_size - 1, "lo");
    } else if (if_nametoindex("lo0") > 0) {
        cout << "This is BSD, using lo0 as loopback." << endl;
        snprintf(LOOPBACK, buf_size - 1, "lo0");
    } else {
        cout << "Failed to detect loopback interface. Neither "
             << "lo nor lo0 worked. I give up." << endl;
        FAIL();
    }
}

// uncomment this test to create packet writer. It will
// write incoming DHCPv6 packets as C arrays. That is useful
// for generating test sequences based on actual traffic
//
// TODO: this potentially should be moved to a separate tool
//

#if 0
TEST_F(IfaceMgrTest, dhcp6Sniffer) {
    // testing socket operation in a portable way is tricky
    // without interface detection implemented

    unlink("interfaces.txt");

    ofstream interfaces("interfaces.txt", ios::ate);
    interfaces << "eth0 fe80::21e:8cff:fe9b:7349";
    interfaces.close();

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    Pkt6* pkt = NULL;
    int cnt = 0;
    cout << "---8X-----------------------------------------" << endl;
    while (true) {
        pkt = ifacemgr->receive();

        cout << "// this code is autogenerated. Do NOT edit." << endl;
        cout << "// Received " << pkt->data_len_ << " bytes packet:" << endl;
        cout << "Pkt6 *capture" << cnt++ << "() {" << endl;
        cout << "    Pkt6* pkt;" << endl;
        cout << "    pkt = new Pkt6(" << pkt->data_len_ << ");" << endl;
        cout << "    pkt->remote_port_ = " << pkt-> remote_port_ << ";" << endl;
        cout << "    pkt->remote_addr_ = IOAddress(\""
             << pkt->remote_addr_.toText() << "\");" << endl;
        cout << "    pkt->local_port_ = " << pkt-> local_port_ << ";" << endl;
        cout << "    pkt->local_addr_ = IOAddress(\""
             << pkt->local_addr_.toText() << "\");" << endl;
        cout << "    pkt->ifindex_ = " << pkt->ifindex_ << ";" << endl;
        cout << "    pkt->iface_ = \"" << pkt->iface_ << "\";" << endl;

        // TODO it is better to declare statically initialize the array
        // and then memcpy it to packet.
        for (int i=0; i< pkt->data_len_; i++) {
            cout << "    pkt->data_[" << i << "]="
                 << (int)(unsigned char)pkt->data_[i] << "; ";
            if (!(i%4))
                cout << endl;
        }
        cout << endl;
        cout << "    return (pkt);" << endl;
        cout << "}" << endl << endl;

        delete pkt;
    }
    cout << "---8X-----------------------------------------" << endl;

    // never happens. Infinite loop is infinite
    delete pkt;
    delete ifacemgr;
}
#endif

TEST_F(IfaceMgrTest, basic) {
    // checks that IfaceManager can be instantiated

    IfaceMgr & ifacemgr = IfaceMgr::instance();
    ASSERT_TRUE(&ifacemgr != 0);
}

TEST_F(IfaceMgrTest, ifaceClass) {
    // basic tests for Iface inner class

    IfaceMgr::Iface* iface = new IfaceMgr::Iface("eth5", 7);

    EXPECT_STREQ("eth5/7", iface->getFullName().c_str());

    delete iface;
}

// TODO: Implement getPlainMac() test as soon as interface detection
// is implemented.
TEST_F(IfaceMgrTest, getIface) {

    cout << "Interface checks. Please ignore socket binding errors." << endl;
    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    // interface name, ifindex
    IfaceMgr::Iface iface1("lo1", 100);
    IfaceMgr::Iface iface2("eth9", 101);
    IfaceMgr::Iface iface3("en3", 102);
    IfaceMgr::Iface iface4("e1000g4", 103);
    cout << "This test assumes that there are less than 100 network interfaces"
         << " in the tested system and there are no lo1, eth9, en3, e1000g4"
         << " or wifi15 interfaces present." << endl;

    // note: real interfaces may be detected as well
    ifacemgr->getIfacesLst().push_back(iface1);
    ifacemgr->getIfacesLst().push_back(iface2);
    ifacemgr->getIfacesLst().push_back(iface3);
    ifacemgr->getIfacesLst().push_back(iface4);

    cout << "There are " << ifacemgr->getIfacesLst().size()
         << " interfaces." << endl;
    for (IfaceMgr::IfaceCollection::iterator iface=ifacemgr->getIfacesLst().begin();
         iface != ifacemgr->getIfacesLst().end();
         ++iface) {
        cout << "  " << iface->getFullName() << endl;
    }


    // check that interface can be retrieved by ifindex
    IfaceMgr::Iface* tmp = ifacemgr->getIface(102);
    ASSERT_TRUE(tmp != NULL);

    EXPECT_EQ("en3", tmp->getName());
    EXPECT_EQ(102, tmp->getIndex());

    // check that interface can be retrieved by name
    tmp = ifacemgr->getIface("lo1");
    ASSERT_TRUE(tmp != NULL);

    EXPECT_EQ("lo1", tmp->getName());
    EXPECT_EQ(100, tmp->getIndex());

    // check that non-existing interfaces are not returned
    EXPECT_EQ(static_cast<void*>(NULL), ifacemgr->getIface("wifi15") );

    delete ifacemgr;

}

TEST_F(IfaceMgrTest, sockets6) {
    // testing socket operation in a portable way is tricky
    // without interface detection implemented

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    IOAddress loAddr("::1");

    Pkt6 pkt6(DHCPV6_SOLICIT, 123);
    pkt6.setIface(LOOPBACK);

    // bind multicast socket to port 10547
    int socket1 = ifacemgr->openSocket(LOOPBACK, loAddr, 10547);
    EXPECT_GT(socket1, 0); // socket > 0

    EXPECT_EQ(socket1, ifacemgr->getSocket(pkt6));

    // bind unicast socket to port 10548
    int socket2 = ifacemgr->openSocket(LOOPBACK, loAddr, 10548);
    EXPECT_GT(socket2, 0);

    // removed code for binding socket twice to the same address/port
    // as it caused problems on some platforms (e.g. Mac OS X)

    close(socket1);
    close(socket2);

    delete ifacemgr;
}

// TODO: disabled due to other naming on various systems
// (lo in Linux, lo0 in BSD systems)
TEST_F(IfaceMgrTest, DISABLED_sockets6Mcast) {
    // testing socket operation in a portable way is tricky
    // without interface detection implemented

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    IOAddress loAddr("::1");
    IOAddress mcastAddr("ff02::1:2");

    // bind multicast socket to port 10547
    int socket1 = ifacemgr->openSocket(LOOPBACK, mcastAddr, 10547);
    EXPECT_GT(socket1, 0); // socket > 0

    // expect success. This address/port is already bound, but
    // we are using SO_REUSEADDR, so we can bind it twice
    int socket2 = ifacemgr->openSocket(LOOPBACK, mcastAddr, 10547);
    EXPECT_GT(socket2, 0);

    // there's no good way to test negative case here.
    // we would need non-multicast interface. We will be able
    // to iterate thru available interfaces and check if there
    // are interfaces without multicast-capable flag.

    close(socket1);
    close(socket2);

    delete ifacemgr;
}

TEST_F(IfaceMgrTest, sendReceive6) {

    // testing socket operation in a portable way is tricky
    // without interface detection implemented

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    // let's assume that every supported OS have lo interface
    IOAddress loAddr("::1");
    int socket1 = 0, socket2 = 0;
    EXPECT_NO_THROW(
        socket1 = ifacemgr->openSocket(LOOPBACK, loAddr, 10547);
        socket2 = ifacemgr->openSocket(LOOPBACK, loAddr, 10546);
    );

    EXPECT_GT(socket1, 0);
    EXPECT_GT(socket2, 0);


    // prepare dummy payload
    uint8_t data[128];
    for (int i = 0; i < 128; i++) {
        data[i] = i;
    }
    Pkt6Ptr sendPkt = Pkt6Ptr(new Pkt6(data, 128));

    sendPkt->repack();

    sendPkt->setRemotePort(10547);
    sendPkt->setRemoteAddr(IOAddress("::1"));
    sendPkt->setIndex(1);
    sendPkt->setIface(LOOPBACK);

    Pkt6Ptr rcvPkt;

    EXPECT_EQ(true, ifacemgr->send(sendPkt));

    rcvPkt = ifacemgr->receive6();

    ASSERT_TRUE(rcvPkt); // received our own packet

    // let's check that we received what was sent
    ASSERT_EQ(sendPkt->getData().size(), rcvPkt->getData().size());
    EXPECT_EQ(0, memcmp(&sendPkt->getData()[0], &rcvPkt->getData()[0],
                        rcvPkt->getData().size()));

    EXPECT_EQ(sendPkt->getRemoteAddr().toText(), rcvPkt->getRemoteAddr().toText());

    // since we opened 2 sockets on the same interface and none of them is multicast,
    // none is preferred over the other for sending data, so we really should not
    // assume the one or the other will always be choosen for sending data. Therefore
    // we should accept both values as source ports.
    EXPECT_TRUE((rcvPkt->getRemotePort() == 10546) || (rcvPkt->getRemotePort() == 10547));

    delete ifacemgr;
}

TEST_F(IfaceMgrTest, sendReceive4) {

    // testing socket operation in a portable way is tricky
    // without interface detection implemented

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    // let's assume that every supported OS have lo interface
    IOAddress loAddr("127.0.0.1");
    int socket1 = 0, socket2 = 0;
    EXPECT_NO_THROW(
        socket1 = ifacemgr->openSocket(LOOPBACK, loAddr, DHCP4_SERVER_PORT + 10000);
        socket2 = ifacemgr->openSocket(LOOPBACK, loAddr, DHCP4_SERVER_PORT + 10000 + 1);
    );

    EXPECT_GE(socket1, 0);
    EXPECT_GE(socket2, 0);

    boost::shared_ptr<Pkt4> sendPkt(new Pkt4(DHCPDISCOVER, 1234) );

    sendPkt->setLocalAddr(IOAddress("127.0.0.1"));

    sendPkt->setLocalPort(DHCP4_SERVER_PORT + 10000 + 1);
    sendPkt->setRemotePort(DHCP4_SERVER_PORT + 10000);
    sendPkt->setRemoteAddr(IOAddress("127.0.0.1"));
    sendPkt->setIndex(1);
    sendPkt->setIface(string(LOOPBACK));
    sendPkt->setHops(6);
    sendPkt->setSecs(42);
    sendPkt->setCiaddr(IOAddress("192.0.2.1"));
    sendPkt->setSiaddr(IOAddress("192.0.2.2"));
    sendPkt->setYiaddr(IOAddress("192.0.2.3"));
    sendPkt->setGiaddr(IOAddress("192.0.2.4"));

    // unpack() now checks if mandatory DHCP_MESSAGE_TYPE is present
    boost::shared_ptr<Option> msgType(new Option(Option::V4,
           static_cast<uint16_t>(DHO_DHCP_MESSAGE_TYPE)));
    msgType->setUint8(static_cast<uint8_t>(DHCPDISCOVER));
    sendPkt->addOption(msgType);

    uint8_t sname[] = "That's just a string that will act as SNAME";
    sendPkt->setSname(sname, strlen((const char*)sname));
    uint8_t file[] = "/another/string/that/acts/as/a/file_name.txt";
    sendPkt->setFile(file, strlen((const char*)file));

    ASSERT_NO_THROW(
        sendPkt->pack();
    );

    boost::shared_ptr<Pkt4> rcvPkt;

    EXPECT_EQ(true, ifacemgr->send(sendPkt));

    rcvPkt = ifacemgr->receive4();

    ASSERT_TRUE(rcvPkt); // received our own packet

    ASSERT_NO_THROW(
        rcvPkt->unpack();
    );

    // let's check that we received what was sent
    EXPECT_EQ(sendPkt->len(), rcvPkt->len());

    EXPECT_EQ("127.0.0.1", rcvPkt->getRemoteAddr().toText());
    EXPECT_EQ(sendPkt->getRemotePort(), rcvPkt->getLocalPort());

    // now let's check content
    EXPECT_EQ(sendPkt->getHops(), rcvPkt->getHops());
    EXPECT_EQ(sendPkt->getOp(),   rcvPkt->getOp());
    EXPECT_EQ(sendPkt->getSecs(), rcvPkt->getSecs());
    EXPECT_EQ(sendPkt->getFlags(), rcvPkt->getFlags());
    EXPECT_EQ(sendPkt->getCiaddr(), rcvPkt->getCiaddr());
    EXPECT_EQ(sendPkt->getSiaddr(), rcvPkt->getSiaddr());
    EXPECT_EQ(sendPkt->getYiaddr(), rcvPkt->getYiaddr());
    EXPECT_EQ(sendPkt->getGiaddr(), rcvPkt->getGiaddr());
    EXPECT_EQ(sendPkt->getTransid(), rcvPkt->getTransid());
    EXPECT_EQ(sendPkt->getType(), rcvPkt->getType());
    EXPECT_TRUE(sendPkt->getSname() == rcvPkt->getSname());
    EXPECT_TRUE(sendPkt->getFile() == rcvPkt->getFile());
    EXPECT_EQ(sendPkt->getHtype(), rcvPkt->getHtype());
    EXPECT_EQ(sendPkt->getHlen(), rcvPkt->getHlen());

    // since we opened 2 sockets on the same interface and none of them is multicast,
    // none is preferred over the other for sending data, so we really should not
    // assume the one or the other will always be choosen for sending data. We should
    // skip checking source port of sent address.

    delete ifacemgr;
}


TEST_F(IfaceMgrTest, socket4) {

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();

    // Let's assume that every supported OS have lo interface.
    IOAddress loAddr("127.0.0.1");
    // Use unprivileged port (it's convenient for running tests as non-root).
    int socket1 = 0;

    EXPECT_NO_THROW(
        socket1 = ifacemgr->openSocket(LOOPBACK, loAddr, DHCP4_SERVER_PORT + 10000);
    );

    EXPECT_GT(socket1, 0);

    Pkt4 pkt(DHCPDISCOVER, 1234);
    pkt.setIface(LOOPBACK);

    // Expect that we get the socket that we just opened.
    EXPECT_EQ(socket1, ifacemgr->getSocket(pkt));

    close(socket1);

    delete ifacemgr;
}

// Test the Iface structure itself
TEST_F(IfaceMgrTest, iface) {
    IfaceMgr::Iface* iface = NULL;
    EXPECT_NO_THROW(
        iface = new IfaceMgr::Iface("eth0",1);
    );

    EXPECT_EQ("eth0", iface->getName());
    EXPECT_EQ(1, iface->getIndex());
    EXPECT_EQ("eth0/1", iface->getFullName());

    // Let's make a copy of this address collection.
    IfaceMgr::AddressCollection addrs = iface->getAddresses();

    EXPECT_EQ(0, addrs.size());

    IOAddress addr1("192.0.2.6");
    iface->addAddress(addr1);

    addrs = iface->getAddresses();
    ASSERT_EQ(1, addrs.size());
    EXPECT_EQ("192.0.2.6", addrs.at(0).toText());

    // No such address, should return false.
    EXPECT_FALSE(iface->delAddress(IOAddress("192.0.8.9")));

    // This address is present, delete it!
    EXPECT_TRUE(iface->delAddress(IOAddress("192.0.2.6")));

    // Not really necessary, previous reference still points to the same
    // collection. Let's do it anyway, as test code may serve as example
    // usage code as well.
    addrs = iface->getAddresses();

    EXPECT_EQ(0, addrs.size());

    EXPECT_NO_THROW(
        delete iface;
    );
}

TEST_F(IfaceMgrTest, iface_methods) {
    IfaceMgr::Iface iface("foo", 1234);

    iface.setHWType(42);
    EXPECT_EQ(42, iface.getHWType());

    uint8_t mac[IfaceMgr::MAX_MAC_LEN+10];
    for (int i = 0; i < IfaceMgr::MAX_MAC_LEN + 10; i++)
        mac[i] = 255 - i;

    EXPECT_EQ("foo", iface.getName());
    EXPECT_EQ(1234, iface.getIndex());

    // MAC is too long. Exception should be thrown and
    // MAC length should not be set.
    EXPECT_THROW(
        iface.setMac(mac, IfaceMgr::MAX_MAC_LEN + 1),
        OutOfRange
    );

    // MAC length should stay not set as excep
    EXPECT_EQ(0, iface.getMacLen());

    // Setting maximum length MAC should be ok.
    iface.setMac(mac, IfaceMgr::MAX_MAC_LEN);

    // For some reason constants cannot be used directly in EXPECT_EQ
    // as this produces linking error.
    size_t len = IfaceMgr::MAX_MAC_LEN;
    EXPECT_EQ(len, iface.getMacLen());
    EXPECT_EQ(0, memcmp(mac, iface.getMac(), iface.getMacLen()));
}

TEST_F(IfaceMgrTest, socketInfo) {

    // check that socketinfo for IPv4 socket is functional
    IfaceMgr::SocketInfo sock1(7, IOAddress("192.0.2.56"), DHCP4_SERVER_PORT + 7);
    EXPECT_EQ(7, sock1.sockfd_);
    EXPECT_EQ("192.0.2.56", sock1.addr_.toText());
    EXPECT_EQ(AF_INET, sock1.family_);
    EXPECT_EQ(DHCP4_SERVER_PORT + 7, sock1.port_);

    // check that socketinfo for IPv6 socket is functional
    IfaceMgr::SocketInfo sock2(9, IOAddress("2001:db8:1::56"), DHCP4_SERVER_PORT + 9);
    EXPECT_EQ(9, sock2.sockfd_);
    EXPECT_EQ("2001:db8:1::56", sock2.addr_.toText());
    EXPECT_EQ(AF_INET6, sock2.family_);
    EXPECT_EQ(DHCP4_SERVER_PORT + 9, sock2.port_);

    // now let's test if IfaceMgr handles socket info properly
    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();
    IfaceMgr::Iface* loopback = ifacemgr->getIface(LOOPBACK);
    ASSERT_TRUE(loopback);
    loopback->addSocket(sock1);
    loopback->addSocket(sock2);

    Pkt6 pkt6(DHCPV6_REPLY, 123456);

    // pkt6 dos not have interface set yet
    EXPECT_THROW(
        ifacemgr->getSocket(pkt6),
        BadValue
    );

    // try to send over non-existing interface
    pkt6.setIface("nosuchinterface45");
    EXPECT_THROW(
        ifacemgr->getSocket(pkt6),
        BadValue
    );

    // this will work
    pkt6.setIface(LOOPBACK);
    EXPECT_EQ(9, ifacemgr->getSocket(pkt6));

    bool deleted = false;
    EXPECT_NO_THROW(
        deleted = ifacemgr->getIface(LOOPBACK)->delSocket(9);
    );
    EXPECT_EQ(true, deleted);

    // it should throw again, there's no usable socket anymore
    EXPECT_THROW(
        ifacemgr->getSocket(pkt6),
        Unexpected
    );

    // repeat for pkt4
    Pkt4 pkt4(DHCPDISCOVER, 1);

    // pkt4 does not have interface set yet.
    EXPECT_THROW(
        ifacemgr->getSocket(pkt4),
        BadValue
    );

    // Try to send over non-existing interface.
    pkt4.setIface("nosuchinterface45");
    EXPECT_THROW(
        ifacemgr->getSocket(pkt4),
        BadValue
    );

    // Socket info is set, packet has well defined interface. It should work.
    pkt4.setIface(LOOPBACK);
    EXPECT_EQ(7, ifacemgr->getSocket(pkt4));

    EXPECT_NO_THROW(
        ifacemgr->getIface(LOOPBACK)->delSocket(7);
    );

    // It should throw again, there's no usable socket anymore.
    EXPECT_THROW(
        ifacemgr->getSocket(pkt4),
        Unexpected
    );

    delete ifacemgr;
}

#if defined(OS_LINUX)

/// @brief parses text representation of MAC address
///
/// This function parses text representation of a MAC address and stores
/// it in binary format. Text format is expecte to be separate with
/// semicolons, e.g. f4:6d:04:96:58:f2
///
/// TODO: IfaceMgr::Iface::mac_ uses uint8_t* type, should be vector<uint8_t>
///
/// @param textMac string with MAC address to parse
/// @param mac pointer to output buffer
/// @param macLen length of output buffer
///
/// @return number of bytes filled in output buffer
size_t parse_mac(const std::string& textMac, uint8_t* mac, size_t macLen) {
    stringstream tmp(textMac);
    tmp.flags(ios::hex);
    int i = 0;
    uint8_t octet = 0; // output octet
    uint8_t byte;  // parsed charater from text representation
    while (!tmp.eof()) {

        tmp >> byte; // hex value
        if (byte == ':') {
            mac[i++] = octet;

            if (i == macLen) {
                // parsing aborted. We hit output buffer size
                return(i);
            }
            octet = 0;
            continue;
        }
        if (isalpha(byte)) {
            byte = toupper(byte) - 'A' + 10;
        } else if (isdigit(byte)) {
            byte -= '0';
        } else {
            // parse error. Let's return what we were able to parse so far
            break;
        }
        octet <<= 4;
        octet += byte;
    }
    mac[i++] = octet;

    return (i);
}

/// @brief Parses 'ifconfig -a' output and creates list of interfaces
///
/// This method tries to parse ifconfig output. Note that there are some
/// oddities in recent versions of ifconfig, like putting extra spaces
/// after MAC address, inconsistent naming and spacing between inet and inet6.
/// This is an attempt to find a balance between tight parsing of every piece
/// of text that ifconfig prints and robustness to handle slight differences
/// in ifconfig output.
///
/// @todo: Consider using isc::util::str::tokens here.
///
/// @param textFile name of a text file that holds output of ifconfig -a
/// @param ifaces empty list of interfaces to be filled
void parse_ifconfig(const std::string& textFile, IfaceMgr::IfaceCollection& ifaces) {
    fstream f(textFile.c_str());

    bool first_line = true;
    IfaceMgr::IfaceCollection::iterator iface;
    while (!f.eof()) {
        string line;
        getline(f, line);

        // interfaces are separated by empty line
        if (line.length() == 0) {
            first_line = true;
            continue;
        }

        // uncomment this for ifconfig output debug
        // cout << "line[" << line << "]" << endl;

        // this is first line of a new interface
        if (first_line) {
            first_line = false;

            size_t offset;
            offset = line.find_first_of(" ");
            if (offset == string::npos) {
                isc_throw(BadValue, "Malformed output of ifconfig");
            }

            // ifconfig in Gentoo prints out eth0: instead of eth0
            if (line[offset - 1] == ':') {
                offset--;
            }
            string name = line.substr(0, offset);

            // sadly, ifconfig does not return ifindex
            ifaces.push_back(IfaceMgr::Iface(name, 0));
            iface = ifaces.end();
            --iface; // points to the last element

            offset = line.find(string("HWaddr"));

            string mac = "";
            if (offset != string::npos) { // some interfaces don't have MAC (e.g. lo)
                offset += 7;
                mac = line.substr(offset, string::npos);
                mac = mac.substr(0, mac.find_first_of(" "));

                uint8_t buf[IfaceMgr::MAX_MAC_LEN];
                int mac_len = parse_mac(mac, buf, IfaceMgr::MAX_MAC_LEN);
                iface->setMac(buf, mac_len);
            }
        }

        if (line.find("inet6") != string::npos) {
            // IPv6 address
            string addr;
            if (line.find("addr:", line.find("inet6")) != string::npos) {
                // Ubuntu style format: inet6 addr: ::1/128 Scope:Host
                addr = line.substr(line.find("addr:") + 6, string::npos);
            } else {
                // Gentoo style format: inet6 fe80::6ef0:49ff:fe96:ba17  prefixlen 64  scopeid 0x20<link>
                addr = line.substr(line.find("inet6") + 6, string::npos);
            }

            // handle Ubuntu format: inet6 addr: fe80::f66d:4ff:fe96:58f2/64 Scope:Link
            addr = addr.substr(0, addr.find("/"));

            // handle inet6 fe80::ca3a:35ff:fed4:8f1d  prefixlen 64  scopeid 0x20<link>
            addr = addr.substr(0, addr.find(" "));
            IOAddress a(addr);
            iface->addAddress(a);
        } else if(line.find("inet") != string::npos) {
            // IPv4 address
            string addr;
            if (line.find("addr:", line.find("inet")) != string::npos) {
                // Ubuntu style format: inet addr:127.0.0.1  Mask:255.0.0.0
                addr = line.substr(line.find("addr:") + 5, string::npos);
            } else {
                // Gentoo style format: inet 10.53.0.4  netmask 255.255.255.0
                addr = line.substr(line.find("inet") + 5, string::npos);
            }

            addr = addr.substr(0, addr.find_first_of(" "));
            IOAddress a(addr);
            iface->addAddress(a);
        } else if(line.find("Metric")) {
            // flags
            if (line.find("UP") != string::npos) {
                iface->flag_up_ = true;
            }
            if (line.find("LOOPBACK") != string::npos) {
                iface->flag_loopback_ = true;
            }
            if (line.find("RUNNING") != string::npos) {
                iface->flag_running_ = true;
            }
            if (line.find("BROADCAST") != string::npos) {
                iface->flag_broadcast_ = true;
            }
            if (line.find("MULTICAST") != string::npos) {
                iface->flag_multicast_ = true;
            }
        }
    }
}


// This test compares implemented detection routines to output of "ifconfig -a" command.
// It is far from perfect, but it is able to verify that interface names, flags,
// MAC address, IPv4 and IPv6 addresses are detected properly. Interface list completeness
// (check that each interface is reported, i.e. no missing or extra interfaces) and
// address completeness is verified.
//
// Things that are not tested:
// - ifindex (ifconfig does not print it out)
// - address scopes and lifetimes (we don't need it, so it is not implemented in IfaceMgr)
// TODO: temporarily disabled, see ticket #1529
TEST_F(IfaceMgrTest, DISABLED_detectIfaces_linux) {

    NakedIfaceMgr* ifacemgr = new NakedIfaceMgr();
    IfaceMgr::IfaceCollection& detectedIfaces = ifacemgr->getIfacesLst();

    const std::string textFile = "ifconfig.txt";

    unlink(textFile.c_str());
    int result = system( ("/sbin/ifconfig -a > " + textFile).c_str());

    ASSERT_EQ(0, result);

    // list of interfaces parsed from ifconfig
    IfaceMgr::IfaceCollection parsedIfaces;

    ASSERT_NO_THROW(
        parse_ifconfig(textFile, parsedIfaces);
    );
    unlink(textFile.c_str());

    cout << "------Parsed interfaces---" << endl;
    for (IfaceMgr::IfaceCollection::iterator i = parsedIfaces.begin();
         i != parsedIfaces.end(); ++i) {
        cout << i->getName() << ": ifindex=" << i->getIndex() << ", mac=" << i->getPlainMac();
        cout << ", flags:";
        if (i->flag_up_) {
            cout << " UP";
        }
        if (i->flag_running_) {
            cout << " RUNNING";
        }
        if (i->flag_multicast_) {
            cout << " MULTICAST";
        }
        if (i->flag_broadcast_) {
            cout << " BROADCAST";
        }
        cout << ", addrs:";
        const IfaceMgr::AddressCollection& addrs = i->getAddresses();
        for (IfaceMgr::AddressCollection::const_iterator a= addrs.begin();
             a != addrs.end(); ++a) {
            cout << a->toText() << " ";
        }
        cout << endl;
    }

    // Ok, now we have 2 lists of interfaces. Need to compare them
    ASSERT_EQ(detectedIfaces.size(), parsedIfaces.size());

    // TODO: This could could probably be written simple with find()
    for (IfaceMgr::IfaceCollection::iterator detected = detectedIfaces.begin();
         detected != detectedIfaces.end(); ++detected) {
        // let's find out if this interface is

        bool found = false;
        for (IfaceMgr::IfaceCollection::iterator i = parsedIfaces.begin();
             i != parsedIfaces.end(); ++i) {
            if (detected->getName() != i->getName()) {
                continue;
            }
            found = true;

            cout << "Checking interface " << detected->getName() << endl;

            // start with checking flags
            EXPECT_EQ(detected->flag_loopback_, i->flag_loopback_);
            EXPECT_EQ(detected->flag_up_, i->flag_up_);
            EXPECT_EQ(detected->flag_running_, i->flag_running_);
            EXPECT_EQ(detected->flag_multicast_, i->flag_multicast_);
            EXPECT_EQ(detected->flag_broadcast_, i->flag_broadcast_);

            // skip MAC comparison for loopback as netlink returns MAC
            // 00:00:00:00:00:00 for lo
            if (!detected->flag_loopback_) {
                ASSERT_EQ(detected->getMacLen(), i->getMacLen());
                EXPECT_EQ(0, memcmp(detected->getMac(), i->getMac(), i->getMacLen()));
            }

            EXPECT_EQ(detected->getAddresses().size(), i->getAddresses().size());

            // now compare addresses
            const IfaceMgr::AddressCollection& addrs = detected->getAddresses();
            for (IfaceMgr::AddressCollection::const_iterator addr = addrs.begin();
                 addr != addrs.end(); ++addr) {
                bool addr_found = false;

                const IfaceMgr::AddressCollection& addrs2 = detected->getAddresses();
                for (IfaceMgr::AddressCollection::const_iterator a = addrs2.begin();
                     a != addrs2.end(); ++a) {
                    if (*addr != *a) {
                        continue;
                    }
                    addr_found = true;
                }
                if (!addr_found) {
                    cout << "ifconfig does not seem to report " << addr->toText()
                         << " address on " << detected->getFullName() << " interface." << endl;
                    FAIL();
                }
                cout << "Address " << addr->toText() << " on iterface " << detected->getFullName()
                     << " matched with 'ifconfig -a' output." << endl;
            }
        }
        if (!found) { // corresponding interface was not found
            FAIL();
        }
    }

    delete ifacemgr;
}
#endif

}
