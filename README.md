# Steel Battalion Controller Library for Arduino
[![arduino-library-badge](https://www.ardu-badge.com/badge/SBC.svg?)](https://www.ardu-badge.com/SBC)

Allows communications with a Steel Battalion Controller using a Teensy 4.0 or 4.1

Requires an original XBOX -> USB adapter.  Make sure it is no longer than 3 ft.

Additionally the library currently only works when plugged in directly into a computer.  It does NOT work when plugged in through a USB hub.
This seems to be a power negotiation issue, but I have not been able to resolve it.  Also will not work if a hub is used between the Teensy and the controller.


## License

Copyright (c) 2010 Arduino LLC. All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
