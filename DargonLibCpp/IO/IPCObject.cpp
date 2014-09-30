#include "../dlc_pch.hpp"
#include <string>
#include <iostream>
#include <boost/asio.hpp>

#ifdef WIN32
#include <Windows.h>
#endif

#include "../Util.hpp"
#include "IPCObject.hpp"

using namespace Dargon::IO;
using Dargon::Util::Logger;

IPCObject::IPCObject()
   : m_lastError(0), m_pipeHandle(0)
{
}

bool IPCObject::Open(IN std::string pipeName, 
                     IN Dargon::IO::FileAccess accessMode,
                     IN Dargon::IO::FileShare shareMode,
                     IN bool writesBuffered) 
{
#ifdef WIN32
   std::string pipePath = "\\\\.\\pipe\\" + pipeName;
   m_pipeHandle = CreateFileA(
      pipePath.c_str(),
      GENERIC_READ | GENERIC_WRITE, 
      0,
      NULL, 
      OPEN_EXISTING, 
      FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED,
      0
   );
   
   if(m_pipeHandle == INVALID_HANDLE_VALUE)
   {
      m_lastError = ::GetLastError();
      return false;
   }

   std::stringstream ss; ss << "Named Pipe Handle " << m_pipeHandle;
   return true;
#endif
}

UINT32 IPCObject::GetLastError()
{
   return m_lastError;
}

void IPCObject::Close()
{
}

//-------------------------------------------------------------------------------------------------
// IO Read/Write Operations
//-------------------------------------------------------------------------------------------------
Dargon::Blob* IPCObject::ReadBytes(IN boost::uint32_t numBytes)
{
#ifdef WIN32
   return nullptr;
   //BYTE* data = new BYTE[numBytes];
   //DWORD bytesRead;
   //if(!ReadFile(m_pipeHandle, data, numBytes, &bytesRead, nullptr))
   //{
   //   m_lastError = GetLastError();
   //   delete data;
   //   return nullptr;
   //}
   //else
   //{
   //   return new Blob(numBytes, data);
   //}
#endif
}

bool IPCObject::ReadBytes(OUT             void* buffer, 
                          IN              boost::uint32_t numBytes, 
                          OUT OPTIONAL    boost::uint32_t* paramBytesRead)
{
   Logger::L(LL_VERBOSE, [=](std::ostream& os){ os << "Read: buffer " << buffer << " numBytes " << numBytes << std::endl; });

   static_assert(sizeof(DWORD) == sizeof(boost::uint32_t), "DWORD size isn't same as UINT32 size!");
   if(numBytes == 0) return true;
   if(paramBytesRead != nullptr) *paramBytesRead = numBytes;

#ifdef WIN32
   OVERLAPPED overlapped;
   UINT32 totalBytesRead = 0;
   UINT32 totalBytesToRead = numBytes;

   do
   {
      UINT32 totalBytesToTransfer = totalBytesToRead - totalBytesRead;

      ZeroMemory(&overlapped, sizeof(overlapped));
      overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

      BOOL readFileResult;
      DWORD readFileBytesRead = 0;
      if(!(readFileResult = ReadFile(m_pipeHandle, (BYTE*)buffer + totalBytesRead, totalBytesToTransfer, &readFileBytesRead, &overlapped)))
      {
         m_lastError = ::GetLastError();
         if(m_lastError != ERROR_IO_PENDING)
         {
            Logger::SNL(LL_ERROR, [=](std::ostream& os) { os << "Read byte failed with error " << m_lastError << std::endl; });//; read " << bytesRead << " of " << numBytes << " remaining bytes." << std::endl; });
            CloseHandle(overlapped.hEvent);
            return false;
         }
         else 
         {
            //std::cout << "ReadBytes: Error code ERROR_IO_PENDING is okay " << std::endl;
         }
      }
      
      DWORD bytesTransferred = 0;
      if(readFileResult) //Only wait for overlapped if readfile returned FALSE (else, it was a synchronous call)
         bytesTransferred = readFileBytesRead;
      else
      {
         //std::cout << "ReadBytes: Waiting for single object" << std::endl;
         DWORD waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
         if(waitResult != WAIT_OBJECT_0)
            Logger::SNL(LL_ERROR, [=](std::ostream& os){ os << "IPC Object Read Wait for Single Object returned " << std::hex << waitResult << std::endl; });
         
         //std::cout << "ReadBytes: Getting overlapped result" << std::endl;
         if(!GetOverlappedResult(m_pipeHandle, &overlapped, &bytesTransferred, TRUE))
         {
            m_lastError = ::GetLastError();
            Logger::SNL(LL_ERROR, [=](std::ostream& os){ os << "Get Overlapped IO Result failed with error " << m_lastError << std::endl; });
            CloseHandle(overlapped.hEvent);
            return false;
         }
      }
      CloseHandle(overlapped.hEvent);
      //std::cout << "Bytes Transferred " << bytesTransferred << " Is Complete? " << HasOverlappedIoCompleted(&overlapped) 
      //          << " buffer[0]: " << std::hex << (int)*(BYTE*)buffer << "(" << std::dec << (int)*(BYTE*)buffer << ")" << std::endl;

      totalBytesRead += bytesTransferred;
   } while(totalBytesRead < totalBytesToRead);

   //for(int i = 0; i < totalBytesToRead; i++)
   //{
   //   int value = *((BYTE*)buffer + i);
   //   Logger::L(LL_VERBOSE, [=](std::ostream& os){ os << "R " << i << ": " << std::hex << std::setw(2) << value << std::dec << " (" << value << ") '" << (char)value << "'" << std::endl; });
   //}

      //Logger::SNL(LL_VERBOSE, [=](std::ostream& os){ 
      //   os << "Read Bytes: ";
      //   if(bytesRead > 0)
      //   {
      //      os << (int)*((BYTE*)buffer);
      //      for(UINT32 n = 1; n < bytesRead; n++)
      //         os << ", " << (int)*((BYTE*)buffer + n); 
      //   }
      //   else
      //   {
      //      os << "[none]";
      //   }
      //   os << std::endl;
      //});

      //buffer = (BYTE*)buffer + bytesRead;
      //numBytes -= bytesRead;
      //Logger::SNL(LL_VERBOSE, [=](std::ostream& os) { os << "Read byte count " << bytesRead << ", " << numBytes << " remaining" << std::endl; });
   //}
   return true;
#endif
}

bool IPCObject::Write(IN const void* buffer, 
                      IN boost::uint32_t numBytes)
{
   return Write(buffer, 0, numBytes, nullptr);
}

bool IPCObject::Write(IN           const void* buffer, 
                      IN           boost::uint32_t offset, 
                      IN           boost::uint32_t numBytes,
                      OUT OPTIONAL boost::uint32_t* bytesWritten)
{
   if(numBytes == 0)
   {
      if(bytesWritten)
         *bytesWritten = 0;
      return true;
   }
   //for(int i = 0; i < numBytes; i++)
   //   Logger::L(LL_VERBOSE, [=](std::ostream& os){ os << "W " << i << ": " << (int)*((BYTE*)buffer + i) << " (" << (char)*((BYTE*)buffer + i) << ")" << std::endl; });

#ifdef WIN32
   static_assert(sizeof(DWORD) == sizeof(boost::uint32_t), "DWORD size isn't same as UINT32 size!");
   //Logger::L(LL_VERBOSE, [=](std::ostream& os){ os << "Write offset " << offset << " numBytes " << numBytes << std::endl; });

   OVERLAPPED overlapped;
   ZeroMemory(&overlapped, sizeof(OVERLAPPED));
   overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

   BOOL writeFileResult;
   if(!(writeFileResult = WriteFile(m_pipeHandle, (BYTE*)buffer + offset, numBytes, nullptr, &overlapped)))
   {
      m_lastError = ::GetLastError();
      if(m_lastError != ERROR_IO_PENDING)
      {
         Logger::SNL(LL_ERROR, [this](std::ostream& os){ os << "IPC Write File error " << m_lastError << std::endl; });
         CloseHandle(overlapped.hEvent);
         return false;
      }
      else
      {
         std::cout << "WriteBytes: Error code ERROR_IO_PENDING is okay " << std::endl;
      }
   }
   
   if(!writeFileResult) // If it returns true, the op was synchronous and we shouldn't GetOverlappedResult
   {
      //std::cout << "WriteBytes: Waiting for overlapped event" << std::endl;
      DWORD waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
      if(waitResult != WAIT_OBJECT_0)
         Logger::SNL(LL_ERROR, [=](std::ostream& os){ os << "IPC Object Write Wait for Single Object returned " << std::hex << waitResult << std::endl; });

      DWORD bytesTransferred = 0;
      //std::cout << "WriteBytes: Getting overlapped event results" << std::endl;
      if(!GetOverlappedResult(m_pipeHandle, &overlapped, &bytesTransferred, TRUE))
      {
         m_lastError = ::GetLastError();
         Logger::SNL(LL_ERROR, [this](std::ostream& os){ os << "IPC Write Get Overlapped Result error " << m_lastError << std::endl; });
         CloseHandle(overlapped.hEvent);
         return false;
      }
      //std::cout << "Bytes Transferred " << bytesTransferred << " Is Complete? " << HasOverlappedIoCompleted(&overlapped) << std::endl;
   }
   else
   {
      //std::cout << "All Bytes Transferred (Was Synchronous)" << std::endl;
   }
   CloseHandle(overlapped.hEvent);
   return true;
#endif
}