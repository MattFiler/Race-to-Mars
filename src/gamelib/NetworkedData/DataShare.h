#ifndef PROJECT_DATASHARE_H
#define PROJECT_DATASHARE_H

#include "NetworkedData.h"

/*
 *
 * This is a new struct which I'm going to move our current networking
 * implementation across to - instead of having a function with a giant number
 * of parameters, we'll just pass this struct around and add to it as required.
 * Much neater!
 *
 * This will also stop the potential memory errors of going out of non-confined
 * array bounds in the current implementation.
 *
 */

struct DataShare
{
  /* For instantiating a DataShare that will receive networked data */
  DataShare() { data_type = data_roles::NO_ROLE; }

  /* Initialise the data type for adding data to send */
  DataShare(data_roles _type) { data_type = _type; }

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
  int data_array[20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
};

#endif // PROJECT_DATASHARE_H
