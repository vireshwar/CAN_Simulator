//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package fico4omnet.examples.can.busoff_test;

//
// TODO auto-generated type
//
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;

network Busoff_test1
{
    @display("bgb=454,121");
    submodules:
        bus: CanBus {
            @display("p=272,45");
            gates:
                gate[3];
        }
        node[3]: CanNode;
    connections:
        bus.gate[0] <--> node[0].gate;
        bus.gate[1] <--> node[1].gate;
        bus.gate[2] <--> node[2].gate;
}
