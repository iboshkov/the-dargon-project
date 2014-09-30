#pragma once

#include <string>
#include <boost/integer.hpp>
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include "../Dargon.hpp"
#include "IOTypedefs.hpp"

namespace Dargon { namespace IO {
   class IPCObject : boost::noncopyable
   {
   public:
      /// <summary>
      /// Initializes a new instance of an Inter-process Communication object.  
      /// </summary>
      IPCObject();

      /// <summary>
      /// Creates a new instance of a Dargon IPC object with the given name.
      /// </summary>
      /// <param name="name">
      /// The name of our IPC Object.  
      /// If named pipes are supported, we will try the path "\\.\pipe\pipeName".
      /// Otherwise, we will create a connection with the Dargon Service and associate it with the
      /// given IPC object name.
      /// </param>
      /// <param name="accessMode">
      /// The type of file access we will be performing on the IPC Object.  
      /// </param>
      /// <param name="shareMode">
      /// The type of access that will be permitted to other applications while this object is 
      /// created.
      /// </param>
      /// <param name="buffered">
      /// Whether or not the Write Operations performed on this object will be buffered.  See 
      /// WinAPI's documentation on FILE_FLAG_WRITE_THROUGH for more information.
      /// </param>
      /// <returns>Whether or not the operation was successful</returns
      bool Open(IN std::string pipeName, 
                IN Dargon::IO::FileAccess accessMode,
                IN Dargon::IO::FileShare shareMode,
                IN bool writesBuffered);

      /// <summary>
      /// Gets the last error of this IPC object. The error codes returned are OS-specific.
      /// </summary>
      UINT32 GetLastError();

      /// <summary>
      /// Closes the IPC Stream
      /// </summary>
      void Close();
      
      //-------------------------------------------------------------------------------------------
      // IO Read/Write Operations
      //-------------------------------------------------------------------------------------------
      /// <summary>
      /// Tries to read the given number of bytes and returns a pointer to a Dargon BLOB if 
      /// successful, or NULL if unsuccessful.  The returned BLOB might not be of the requested
      /// number of bytes if we reach the end of the stream; if that occurs, a BLOB of a smaller
      /// size is returned.
      /// 
      /// You are responsible for deleting the returned Blob.
      /// </summary>
      /// <param name="numBytes">
      /// The number of bytes to read
      /// </param>
      /// <returns>
      /// A Blob of less than or equal to the given number of requested bytes.  Returns NULL if the
      /// function fails.  The error can then be accessed via the GetLastError() member method;
      /// </returns>
      Dargon::Blob* ReadBytes(IN boost::uint32_t numBytes);

      /// <summary>
      /// Performs a read operation on the IPC Object.
      /// </summary>
      bool ReadBytes(OUT             void* buffer, 
                     IN              UINT32 numBytes, 
                     OUT OPTIONAL    UINT32* bytesRead = NULL);
      
      /// <summary>
      /// Writes the given buffer to the IPC object's underlying stream using the given offset and
      /// length.
      /// </summary>
      bool Write(IN const void* buffer, 
                 IN boost::uint32_t numBytes);

      /// <summary>
      /// Writes the given buffer to the IPC object's underlying stream using the given offset and
      /// length.
      /// </summary>
      bool Write(IN           const void* buffer, 
                 IN           UINT32 offset, 
                 IN           UINT32 numBytes,
                 OUT OPTIONAL UINT32* bytesWritten = nullptr);

   private: 
      UINT32 m_lastError;

      #ifdef WIN32
      HANDLE m_pipeHandle;
      #endif
   };
} }