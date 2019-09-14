# CSGOTools


## Using CSGOTools
*Windows*

* Clone the repo
* Build the project `libprotobuf` from `protobuf` folder (it should be `protobuf-2.5.0`)
* Download the [Protobuf Compiler](https://github.com/google/protobuf/releases/download/v2.5.0/protoc-2.5.0-win32.zip) and put the content on the protoc folder
* Run the `make_cc_protos.bat` to compile the proto's
* Download the the [Steamworks SDK](https://partner.steamgames.com) and extract the content to the `steamworks_sdk` folder

##### Building libprotobuf
The demo project requires you to build libprotobuf as *Multi-threaded (/MT)* for Release 
and *Multi-threaded Debug (/MTd)* for Debug.

