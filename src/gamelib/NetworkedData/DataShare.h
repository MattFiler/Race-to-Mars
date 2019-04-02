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
  /* Initialise the data type */
  DataShare(NetworkedData _type) { data_type = _type; }

  /* Add data to the struct */
  void add(int new_data)
  {
    if (data_index > 20)
    {
      throw "Exceeded data packet length.";
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
    return data_array[index];
  }

 private:
  enum data_type;
  int data_index = 0;
  int data_array[20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
};

#endif // PROJECT_DATASHARE_H
