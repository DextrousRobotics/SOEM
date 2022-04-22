/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oshw.h"

#define MAX_RT_DEVICES 8

/* Return a list of all interfaces and their indices.  */
__maybe_unused static struct if_nameindex* rt_if_nameindex()
{
  FILE *fh;
  char buf[512];
  char dummy[1000];
  struct if_nameindex* nameindex = malloc(sizeof(if_nameindex) * 8);
  memset(nameindex, 0, sizeof(if_nameindex) * 8);

  fh = fopen("/proc/rtnet/devices", "r");
  if (fh) {
    // eat headers.
    fgets(buf, sizeof buf, fh);

    while (fgets(buf, sizeof buf, fh))
    {
      int index;
      char* name = malloc(EC_MAXLEN_ADAPTERNAME);

      __maybe_unused int ret = sscanf(buf, "%d %s %s", &index, name, dummy);
      nameindex[index -1].if_index = index;
      nameindex[index -1].if_name = name;
    }

    fclose(fh);
  }

  return nameindex;
}

/* Free the data returned from if_nameindex.  */
__maybe_unused static void rt_if_freenameindex(struct if_nameindex* ids)
{
  for(int i = 0; ids[i].if_index != 0; i++)
  {
    free(ids[i].if_name);
  }
  free(ids);
  ids = NULL;
}

/**
 * Host to Network byte order (i.e. to big endian).
 *
 * Note that Ethercat uses little endian byte order, except for the Ethernet
 * header which is big endian as usual.
 */
uint16 oshw_htons(uint16 host)
{
   uint16 network = htons (host);
   return network;
}

/**
 * Network (i.e. big endian) to Host byte order.
 *
 * Note that Ethercat uses little endian byte order, except for the Ethernet
 * header which is big endian as usual.
 */
uint16 oshw_ntohs(uint16 network)
{
   uint16 host = ntohs (network);
   return host;
}

/** Create list over available network adapters.
 * @return First element in linked list of adapters
 */
ec_adaptert * oshw_find_adapters(void)
{
   int i;
   struct if_nameindex *ids;
   ec_adaptert * adapter;
   ec_adaptert * prev_adapter;
   ec_adaptert * ret_adapter = NULL;


   /* Iterate all devices and create a local copy holding the name and
    * description.
    */

   ids = rt_if_nameindex ();
   for(i = 0; ids[i].if_index != 0; i++)
   {
      adapter = (ec_adaptert *)malloc(sizeof(ec_adaptert));
      /* If we got more than one adapter save link list pointer to previous
       * adapter.
       * Else save as pointer to return.
       */
      if (i)
      {
         prev_adapter->next = adapter;
      }
      else
      {
         ret_adapter = adapter;
      }

      /* fetch description and name, in Linux we use the same on both */
      adapter->next = NULL;

      if (ids[i].if_name)
      {
          strncpy(adapter->name, ids[i].if_name, EC_MAXLEN_ADAPTERNAME);
          adapter->name[EC_MAXLEN_ADAPTERNAME-1] = '\0';
          strncpy(adapter->desc, ids[i].if_name, EC_MAXLEN_ADAPTERNAME);
          adapter->desc[EC_MAXLEN_ADAPTERNAME-1] = '\0';
      }
      else
      {
         adapter->name[0] = '\0';
         adapter->desc[0] = '\0';
      }

      prev_adapter = adapter;
   }

   rt_if_freenameindex (ids);

   return ret_adapter;
}

/** Free memory allocated memory used by adapter collection.
 * @param[in] adapter = First element in linked list of adapters
 * EC_NOFRAME.
 */
void oshw_free_adapters(ec_adaptert * adapter)
{
   ec_adaptert * next_adapter;
   /* Iterate the linked list and free all elements holding
    * adapter information
    */
   if(adapter)
   {
      next_adapter = adapter->next;
      free (adapter);
      while (next_adapter)
      {
         adapter = next_adapter;
         next_adapter = adapter->next;
         free (adapter);
      }
   }
}
