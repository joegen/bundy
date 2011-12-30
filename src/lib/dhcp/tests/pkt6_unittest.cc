// Copyright (C) 2011  Internet Systems Consortium, Inc. ("ISC")
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
#include <sstream>
#include <arpa/inet.h>
#include <gtest/gtest.h>

#include <asiolink/io_address.h>
#include <dhcp/option.h>
#include <dhcp/pkt6.h>
#include <dhcp/dhcp6.h>

using namespace std;
using namespace isc;
using namespace isc::asiolink;
using namespace isc::dhcp;

namespace {
// empty class for now, but may be extended once Addr6 becomes bigger
class Pkt6Test : public ::testing::Test {
public:
    Pkt6Test() {
    }
};

TEST_F(Pkt6Test, constructor) {
    Pkt6 * pkt1 = new Pkt6(17);

    EXPECT_EQ(pkt1->data_len_, 17);

    delete pkt1;
}

// captured actual SOLICIT packet: transid=0x3d79fb
// options: client-id, in_na, dns-server, elapsed-time, option-request
// this code is autogenerated (see src/bin/dhcp6/tests/iface_mgr_unittest.c)
Pkt6 *capture1() {
    Pkt6* pkt;
    pkt = new Pkt6(98);
    pkt->remote_port_ = 546;
    pkt->remote_addr_ = IOAddress("fe80::21e:8cff:fe9b:7349");
    pkt->local_port_ = 0;
    pkt->local_addr_ = IOAddress("ff02::1:2");
    pkt->ifindex_ = 2;
    pkt->iface_ = "eth0";
    pkt->data_[0]=1;
    pkt->data_[1]=01;     pkt->data_[2]=02;     pkt->data_[3]=03;     pkt->data_[4]=0;
    pkt->data_[5]=1;     pkt->data_[6]=0;     pkt->data_[7]=14;     pkt->data_[8]=0;
    pkt->data_[9]=1;     pkt->data_[10]=0;     pkt->data_[11]=1;     pkt->data_[12]=21;
    pkt->data_[13]=158;     pkt->data_[14]=60;     pkt->data_[15]=22;     pkt->data_[16]=0;
    pkt->data_[17]=30;     pkt->data_[18]=140;     pkt->data_[19]=155;     pkt->data_[20]=115;
    pkt->data_[21]=73;     pkt->data_[22]=0;     pkt->data_[23]=3;     pkt->data_[24]=0;
    pkt->data_[25]=40;     pkt->data_[26]=0;     pkt->data_[27]=0;     pkt->data_[28]=0;
    pkt->data_[29]=1;     pkt->data_[30]=255;     pkt->data_[31]=255;     pkt->data_[32]=255;
    pkt->data_[33]=255;     pkt->data_[34]=255;     pkt->data_[35]=255;     pkt->data_[36]=255;
    pkt->data_[37]=255;     pkt->data_[38]=0;     pkt->data_[39]=5;     pkt->data_[40]=0;
    pkt->data_[41]=24;     pkt->data_[42]=32;     pkt->data_[43]=1;     pkt->data_[44]=13;
    pkt->data_[45]=184;     pkt->data_[46]=0;     pkt->data_[47]=1;     pkt->data_[48]=0;
    pkt->data_[49]=0;     pkt->data_[50]=0;     pkt->data_[51]=0;     pkt->data_[52]=0;
    pkt->data_[53]=0;     pkt->data_[54]=0;     pkt->data_[55]=0;     pkt->data_[56]=18;
    pkt->data_[57]=52;     pkt->data_[58]=255;     pkt->data_[59]=255;     pkt->data_[60]=255;
    pkt->data_[61]=255;     pkt->data_[62]=255;     pkt->data_[63]=255;     pkt->data_[64]=255;
    pkt->data_[65]=255;     pkt->data_[66]=0;     pkt->data_[67]=23;     pkt->data_[68]=0;
    pkt->data_[69]=16;     pkt->data_[70]=32;     pkt->data_[71]=1;     pkt->data_[72]=13;
    pkt->data_[73]=184;     pkt->data_[74]=0;     pkt->data_[75]=1;     pkt->data_[76]=0;
    pkt->data_[77]=0;     pkt->data_[78]=0;     pkt->data_[79]=0;     pkt->data_[80]=0;
    pkt->data_[81]=0;     pkt->data_[82]=0;     pkt->data_[83]=0;     pkt->data_[84]=221;
    pkt->data_[85]=221;     pkt->data_[86]=0;     pkt->data_[87]=8;     pkt->data_[88]=0;
    pkt->data_[89]=2;     pkt->data_[90]=0;     pkt->data_[91]=100;     pkt->data_[92]=0;
    pkt->data_[93]=6;     pkt->data_[94]=0;     pkt->data_[95]=2;     pkt->data_[96]=0;
    pkt->data_[97]=23;
    return (pkt);
}

TEST_F(Pkt6Test, unpack_solicit1) {
    Pkt6 * sol = capture1();

    ASSERT_EQ(true, sol->unpack());

    // check for length
    EXPECT_EQ(98, sol->len() );

    // check for type
    EXPECT_EQ(DHCPV6_SOLICIT, sol->getType() );

    // check that all present options are returned
    EXPECT_TRUE(sol->getOption(D6O_CLIENTID)); // client-id is present
    EXPECT_TRUE(sol->getOption(D6O_IA_NA));    // IA_NA is present
    EXPECT_TRUE(sol->getOption(D6O_ELAPSED_TIME));  // elapsed is present
    EXPECT_TRUE(sol->getOption(D6O_NAME_SERVERS));
    EXPECT_TRUE(sol->getOption(D6O_ORO));

    // let's check that non-present options are not returned
    EXPECT_FALSE(sol->getOption(D6O_SERVERID)); // server-id is missing
    EXPECT_FALSE(sol->getOption(D6O_IA_TA));
    EXPECT_FALSE(sol->getOption(D6O_IAADDR));

    // let's limit verbosity of this test
    // std::cout << sol->toText();

    delete sol;
}

TEST_F(Pkt6Test, packUnpack) {

    Pkt6 * parent = new Pkt6(100);

    parent->setType(DHCPV6_SOLICIT);

    boost::shared_ptr<Option> opt1(new Option(Option::V6, 1));
    boost::shared_ptr<Option> opt2(new Option(Option::V6, 2));
    boost::shared_ptr<Option> opt3(new Option(Option::V6, 100));
    // let's not use zero-length option type 3 as it is IA_NA

    parent->addOption(opt1);
    parent->addOption(opt2);
    parent->addOption(opt3);

    EXPECT_EQ(DHCPV6_SOLICIT, parent->getType());
    int transid = parent->getTransid();
    // transaction-id was randomized, let's remember it

    // calculated length should be 16
    EXPECT_EQ( Pkt6::DHCPV6_PKT_HDR_LEN + 3*Option::OPTION6_HDR_LEN, 
               parent->len() );

    EXPECT_TRUE( parent->pack() );

    //
    EXPECT_EQ( Pkt6::DHCPV6_PKT_HDR_LEN + 3*Option::OPTION6_HDR_LEN, 
               parent->len() );

    // let's delete options from options_ collection
    // they still be defined in packed 
    parent->options_.clear();

    // that that removed options are indeed are gone
    EXPECT_EQ( 4, parent->len() );

    // now recreate options list
    EXPECT_TRUE( parent->unpack() );

    // transid, message-type should be the same as before
    EXPECT_EQ(transid, parent->getTransid());
    EXPECT_EQ(DHCPV6_SOLICIT, parent->getType());
    
    EXPECT_TRUE( parent->getOption(1));
    EXPECT_TRUE( parent->getOption(2));
    EXPECT_TRUE( parent->getOption(100));
    EXPECT_FALSE( parent->getOption(4));
    
    delete parent;
}

TEST_F(Pkt6Test, addGetDelOptions) {
    Pkt6 * parent = new Pkt6(100);

    boost::shared_ptr<Option> opt1(new Option(Option::V6, 1));
    boost::shared_ptr<Option> opt2(new Option(Option::V6, 2));
    boost::shared_ptr<Option> opt3(new Option(Option::V6, 2));

    parent->addOption(opt1);
    parent->addOption(opt2);

    // getOption() test
    EXPECT_EQ(opt1, parent->getOption(1));
    EXPECT_EQ(opt2, parent->getOption(2));

    // expect NULL
    EXPECT_EQ(boost::shared_ptr<Option>(), parent->getOption(4));

    // now there are 2 options of type 2
    parent->addOption(opt3);

    // let's delete one of them
    EXPECT_EQ(true, parent->delOption(2));

    // there still should be the other option 2
    EXPECT_NE(boost::shared_ptr<Option>(), parent->getOption(2));

    // let's delete the other option 2
    EXPECT_EQ(true, parent->delOption(2));

    // no more options with type=2
    EXPECT_EQ(boost::shared_ptr<Option>(), parent->getOption(2));

    // let's try to delete - should fail
    EXPECT_TRUE(false ==  parent->delOption(2));

    delete parent;
}


}