/*
 * VDF.h
 *
 *  Created on: 9 Oct 2016
 *      Author: muttley
 */

#ifndef VDF_H_
#define VDF_H_

#include "VDFKey.h"
#include <vector>

#define KEY_NOT_FOUND "KEY_WAS_NOT_FOUND_IN_VDF_FILE"

class VDF
{
    public:
        VDF(std::string filename);
        std::vector<VDFKey> Keys;
        std::string GetValue(std::string KeyName);
};

#endif /* VDF_H_ */
