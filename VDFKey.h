/*
 * VDFKey.h
 *
 *  Created on: 9 Oct 2016
 *      Author: muttley
 */

#ifndef VDFKEY_H_
#define VDFKEY_H_

#include <string>

class VDFKey
{
    public:
        VDFKey(std::string path, std::string value);

        std::string Path, Value;
};

#endif /* VDFKEY_H_ */
