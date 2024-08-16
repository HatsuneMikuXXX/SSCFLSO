#ifndef FORMAT_MSG_H
#define FORMAT_MSG_H

const std::string format_msg = R"~~~~(
Format for SSCFLSO instances. Important, data is zero-based 
(For example, having facilities 1,2,3 or a,b,c or 2,5,7 needs to be encoded with 0,1,2):
[Number of facilities] TAB [Number of client]

[Demand of client 1] TAB [Demand of client 2] TAB ... TAB [Demand of client n]

[Capacity of facility 1] TAB ... TAB [Capacity of facility m]

[Cost of facility 1] TAB ... TAB [Cost of facility m]

[Supply cost of facility 1 to client 1] TAB ... TAB [Supply cost of facility 1 to client n]
[Supply cost of facility 2 to client 1] TAB ... TAB [Supply cost of facility 2 to client n]
...
[Supply cost of facility m to client 1] TAB ... TAB [Supply cost of facility m to client n]

[Preferences of client 1]
[Preferences of client 2]
...
[Preferences of client n]
)~~~~";
#endif