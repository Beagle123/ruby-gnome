# Copyright (C) 2015-2023  Ruby-GNOME Project Team
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

module Vte
  class Pty
    alias_method :initialize_raw, :initialize
    def initialize(*args)
      case args[0]
      when PtyFlags
        initialize_new_sync(*args)
      else
        initialize_raw(*args)
      end
    end

    alias_method :size_raw, :size
    def size
      succeeded, rows, columns = size_raw
      if succeeded
        [rows, columns]
      else
        nil
      end
    end
  end
end
