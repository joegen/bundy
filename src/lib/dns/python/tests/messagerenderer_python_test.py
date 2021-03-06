# Copyright (C) 2010  Internet Systems Consortium.
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SYSTEMS CONSORTIUM
# DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
# INTERNET SYSTEMS CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
# FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#
# Tests for the messagerenderer part of the pydnspp module
#

import unittest
import os
from pydnspp import *

class MessageRendererTest(unittest.TestCase):

    def setUp(self):
        name = Name("example.com")
        c = RRClass("IN")
        t = RRType("A")
        ttl = RRTTL("3600")

        message = Message(Message.RENDER)
        message.set_qid(123)
        message.set_opcode(Opcode.QUERY)
        message.set_rcode(Rcode.NOERROR)
        message.add_question(Question(name, c, t))

        self.message1 = message
        message = Message(Message.RENDER)
        message.set_qid(123)
        message.set_header_flag(Message.HEADERFLAG_AA, True)
        message.set_header_flag(Message.HEADERFLAG_QR, True)
        message.set_opcode(Opcode.QUERY)
        message.set_rcode(Rcode.NOERROR)
        message.add_question(Question(name, c, t))
        rrset = RRset(name, c, t, ttl)
        rrset.add_rdata(Rdata(t, c, "192.0.2.98"))
        rrset.add_rdata(Rdata(t, c, "192.0.2.99"))
        message.add_rrset(Message.SECTION_AUTHORITY, rrset)
        self.message2 = message

        self.renderer1 = MessageRenderer()
        self.renderer2 = MessageRenderer()
        self.renderer3 = MessageRenderer()
        self.renderer3.set_length_limit(50)
        self.message1.to_wire(self.renderer1)
        self.message2.to_wire(self.renderer2)
        self.message2.to_wire(self.renderer3)


    def test_messagerenderer_get_data(self):
        data1 = b'\x00{\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x07example\x03com\x00\x00\x01\x00\x01'
        self.assertEqual(data1, self.renderer1.get_data())
        data2 = b'\x00{\x84\x00\x00\x01\x00\x00\x00\x02\x00\x00\x07example\x03com\x00\x00\x01\x00\x01\xc0\x0c\x00\x01\x00\x01\x00\x00\x0e\x10\x00\x04\xc0\x00\x02b\xc0\x0c\x00\x01\x00\x01\x00\x00\x0e\x10\x00\x04\xc0\x00\x02c'
        self.assertEqual(data2, self.renderer2.get_data())

    def test_messagerenderer_get_length(self):
        self.assertEqual(29, self.renderer1.get_length())
        self.assertEqual(61, self.renderer2.get_length())
        self.assertEqual(45, self.renderer3.get_length())

    def test_messagerenderer_is_truncated(self):
        self.assertFalse(self.renderer1.is_truncated())
        self.assertFalse(self.renderer2.is_truncated())
        self.assertTrue(self.renderer3.is_truncated())

    def test_messagerenderer_get_length_limit(self):
        self.assertEqual(512, self.renderer1.get_length_limit())
        self.assertEqual(512, self.renderer2.get_length_limit())
        self.assertEqual(50, self.renderer3.get_length_limit())

    def test_messagerenderer_get_compress_mode(self):
        self.assertEqual(MessageRenderer.CASE_INSENSITIVE,
                         self.renderer1.get_compress_mode())
        self.assertEqual(MessageRenderer.CASE_INSENSITIVE,
                         self.renderer2.get_compress_mode())
        self.assertEqual(MessageRenderer.CASE_INSENSITIVE,
                         self.renderer3.get_compress_mode())

    def test_messagerenderer_set_truncated(self):
        self.assertFalse(self.renderer1.is_truncated())
        self.renderer1.set_truncated()
        self.assertTrue(self.renderer1.is_truncated())

    def test_messagerenderer_set_length_limit(self):
        renderer = MessageRenderer()
        self.assertEqual(512, renderer.get_length_limit())
        renderer.set_length_limit(1024)
        self.assertEqual(1024, renderer.get_length_limit())
        self.assertRaises(TypeError, renderer.set_length_limit, "wrong")
        # Range check.  We need to do this at the binding level, so we need
        # explicit tests for it.
        renderer.set_length_limit(0)
        self.assertEqual(0, renderer.get_length_limit())
        self.assertRaises(ValueError, renderer.set_length_limit, -1)

    def test_messagerenderer_set_compress_mode(self):
        renderer = MessageRenderer()
        self.assertEqual(MessageRenderer.CASE_INSENSITIVE,
                         renderer.get_compress_mode())
        renderer.set_compress_mode(MessageRenderer.CASE_SENSITIVE)
        self.assertEqual(MessageRenderer.CASE_SENSITIVE,
                         renderer.get_compress_mode())
        renderer.set_compress_mode(MessageRenderer.CASE_INSENSITIVE)
        self.assertEqual(MessageRenderer.CASE_INSENSITIVE,
                         renderer.get_compress_mode())
        self.assertRaises(TypeError, renderer.set_compress_mode, "wrong")

if __name__ == '__main__':
    unittest.main()
