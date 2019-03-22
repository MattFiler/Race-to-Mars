//
// Created by j42-king on 19/03/2019.
//

#ifndef PROJECT_MESSAGETYPES_H
#define PROJECT_MESSAGETYPES_H

// These enums classify what type of message is being sent over the network and
// will ALWAYS be the first piece of data in
// the packet.
enum packet_type
{
  PACKET_DEFAULT,
  PACKET_MSG,
  PACKET_CARD

};

#endif // PROJECT_MESSAGETYPES_H
