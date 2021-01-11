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

package fico4omnet.examples.can.busoff_test_SFBO;

//
// TODO auto-generated type
//
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;

network Busoff_test2
{
    @display("bgb=454,121");
    submodules:
        bus: CanBus {
            @display("p=272,45");
            gates:
                gate[3];
        }
        Victim: CanNode {
            @display("p=352.5,19.375");
        }
        Adversary: CanNode {
            @display("p=200.625,24.375");
        }
        Receiver: CanNode {
            @display("p=271.875,98.125");
        }
    connections:
        bus.gate[0] <--> Victim.gate;
        bus.gate[1] <--> Adversary.gate;
        bus.gate[2] <--> Receiver.gate;
}
