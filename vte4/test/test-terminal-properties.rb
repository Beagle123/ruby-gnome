# Copyright (C) 2014-2023  Ruby-GNOME Project Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

class TestTerminalProperties < Test::Unit::TestCase
  include VteTestUtils

  def setup
    @terminal = Vte::Terminal.new
  end

  def test_allow_bold
    @terminal.allow_bold = false
    assert_false(@terminal.allow_bold?)
  end

  def test_audible_bell
    @terminal.audible_bell = false
    assert_false(@terminal.audible_bell?)
  end

  def test_backspace_binding
    bind = Vte::EraseBinding::DELETE_SEQUENCE
    @terminal.backspace_binding = bind
    assert_equal(bind, @terminal.backspace_binding)
  end

  def test_cursor_blink_mode
    cursor_mode = Vte::CursorBlinkMode::ON
    @terminal.cursor_blink_mode = cursor_mode
    assert_equal(cursor_mode, @terminal.cursor_blink_mode)
  end

  def test_cursor_shape
    shape = Vte::CursorShape::IBEAM
    @terminal.cursor_shape = shape
    assert_equal(shape, @terminal.cursor_shape)
  end

  def test_delete_binding
    bind = Vte::EraseBinding::ASCII_DELETE
    @terminal.delete_binding = bind
    assert_equal(bind, @terminal.delete_binding)
  end

  def test_encoding
    encoding = "UTF-16"
    @terminal.encoding = encoding
    assert_equal(encoding, @terminal.encoding)
  end

  def test_font_desc
    font = Pango::FontDescription.new("Monospace 15")
    @terminal.font_desc = font
    assert_equal(font, @terminal.font_desc)
  end

  def test_pointer_autohide
    hide = true
    @terminal.pointer_autohide = hide
    assert_true(@terminal.pointer_autohide?)
  end

  def test_scroll_on_keystroke
    scroll = true
    @terminal.scroll_on_keystroke = scroll
    assert_true(@terminal.scroll_on_keystroke?)
  end

  def test_scroll_on_output
    scroll = false
    @terminal.scroll_on_output = scroll
    assert_false(@terminal.scroll_on_output?)
  end

  def test_scrollback_lines
    lines = 1024
    @terminal.scrollback_lines = lines
    assert_equal(lines, @terminal.scrollback_lines)
  end
end
