#ifndef PROJECT_DATASHARE_H
#define PROJECT_DATASHARE_H

#include "NetworkedData.h"
#include <cstring>
#include <enet/types.h>
#include <string>
#include <vector>

/*
 *
 * This is the foundation of networking within the game.
 *
 * A DataShare struct is created and then sent within the Packet class, which
 * when received, can be read out again.
 *
 * Data should be added to the DataShare using the add() method, and initialised
 * with a data_type from NetworkedData.h. Data can be retrieved from the
 * DataShare using the retrieve() method.
 *
 * A constructor without initialisation exists, but this should be used for
 * streaming into the DataShare when retrieving data from the network.
 *
 * If data is added outside of the bounds of the struct, or retrieved outside
 * the bounds of the contained data, an error will be thrown.
 *
 * Similarly if the DataShare is not constructed properly, it will not allow the
 * ability to add/retrieve data.
 *
 * Further functionality may be useful in future, such as locking the struct to
 * disable data being added.
 *
 */

struct DataShare
{
  /* For instantiating a DataShare that will receive networked data */
  DataShare()
  {
    data_type = data_roles::NO_ROLE;
    chat_msg_data[0] = '\0';
  }

  /* Initialise the data type for adding data to send */
  explicit DataShare(data_roles _type) { data_type = _type; }

  /* Add data to the struct */
  void add(int new_data)
  {
    if (data_index > 20)
    {
      throw "Exceeded data packet length.";
    }
    if (data_type == data_roles::NO_ROLE)
    {
      throw "Tried to add to DataShare of unknown type.";
    }
    data_array[data_index] = new_data;
    data_index++;
  }

  /* Add a message for chat */
  void addMsg(std::string& chat_msg)
  {
    strcpy(chat_msg_data, chat_msg.c_str());
  }

  /* Get a message for chat */
  std::string getMsg()
  {
    size_t length;
    length = strlen(&chat_msg_data[0]);
    std::string msg = std::string(&chat_msg_data[0], length);
    return msg;
  }

  /* Retrieve data from the struct */
  int retrieve(int index)
  {
    if (index > data_index || index < 0)
    {
      throw "Out of packet range.";
    }
    if (data_type == data_roles::NO_ROLE)
    {
      throw "Tried to read DataShare of unknown type.";
    }
    return data_array[index];
  }

  /* Return the type of this DataShare instance */
  data_roles getType() { return data_type; }

 private:
  data_roles data_type;
  int data_index = 0;
  int data_array[20] = { -1 };
  char chat_msg_data[40];
};

#endif // PROJECT_DATASHARE_H
