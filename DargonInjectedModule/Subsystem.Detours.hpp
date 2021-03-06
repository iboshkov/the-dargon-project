#pragma once

#include "stdafx.h"
#include <functional>
#include <detours.h>

#define  DIM_DECL_LIST(DataType, ListContentName) \
         static list<DataType>* m_my##ListContentName##List; \
         public: \
         static bool Add##ListContentName(DataType callback); \
         static bool Remove##ListContentName(DataType callback);

#define  DIM_IMPL_LIST(ClassName, DataType, ListContentName) \
         list<DataType>* ClassName::m_my##ListContentName##List = new list<DataType>(); \
         bool ClassName::Add##ListContentName(DataType callback) \
         { \
            m_my##ListContentName##List->push_back(callback); \
            return true; \
         } \
         bool ClassName::Remove##ListContentName(DataType callback) \
         { \
            m_my##ListContentName##List->remove(callback); \
            return true; \
         }

#define DIM_INVOKE_EVENT(eventList, DelegateTypeName, ...) eventList(##__VA_ARGS__);
//#define DIM_INVOKE_CANCELLABLE_EVENT(eventList, DelegateTypeName, ...) \
//        list<DelegateTypeName>::iterator it##eventList; \
//        BOOL wasCancelled##eventList = false; \
//        for(it##eventList = eventList->begin(); it##eventList != eventList->end(); it##eventList++) \
//        if((*it##eventList)(##__VA_ARGS__)) { wasCancelled##eventList = true; break; }

#define  DIM_DECL_DETOUR(DetouredMethodName, DelegateTypeName) \
         static DelegateTypeName* m_real##DetouredMethodName;  \
         static DelegateTypeName* m_tramp##DetouredMethodName; \
         public: \
         static void Install##DetouredMethodName##Detour(DWORD* pVtable); \
         static void Uninstall##DetouredMethodName##Detour(); \
         private: 

#define  DIM_IMPL_DETOUR(ClassName, DetouredMethodName, DelegateTypeName, vTableOffset, myDetouredMethodImplementation) \
         DelegateTypeName* ClassName::m_real##DetouredMethodName = NULL; \
         DelegateTypeName* ClassName::m_tramp##DetouredMethodName = NULL; \
         void ClassName::Install##DetouredMethodName##Detour(DWORD* pVtable) \
         { \
            if(m_tramp##DetouredMethodName == NULL) /**/ \
            { /**/ \
               /* file_logger::GetOutputStream(LL_INFO) << "Attempt Detour: " #DetouredMethodName " " << endl; /**/ \
               m_real##DetouredMethodName = (DelegateTypeName*)(pVtable[vTableOffset]); /**/ \
               /* file_logger::GetOutputStream(LL_INFO) << "real" #DetouredMethodName ": " << m_real##DetouredMethodName << endl; /**/ \
               m_tramp##DetouredMethodName = (DelegateTypeName*)DetourFunction((PBYTE)m_real##DetouredMethodName, (PBYTE)&myDetouredMethodImplementation); /**/ \
               /* file_logger::GetOutputStream(LL_INFO) << "Done!" << endl; /**/ \
            }else{ /**/ \
               /* file_logger::GetOutputStream(LL_INFO) << "Already detoured " #DetouredMethodName "!: " << m_real##DetouredMethodName << endl; /**/ \
            } /**/\
         } \
         void ClassName::Uninstall##DetouredMethodName##Detour() \
         { \
            if(m_tramp##DetouredMethodName != NULL) \
            { \
               /*DetourRemove((BYTE*)m_tramp##DetouredMethodName, (BYTE*)m_real##DetouredMethodName); /**/ \
            } \
         }

#define  DIM_DECL_STATIC_DETOUR(ClassName, FriendlyFunctionName, DetouredMethodType, DetouredMethodName, TrampolineName) \
         static DetouredMethodType* m_real##FriendlyFunctionName;  \
         static DetouredMethodType* m_tramp##FriendlyFunctionName; \
         public: \
         static void Install##FriendlyFunctionName##Detour(HMODULE hModule); \
         static void Uninstall##FriendlyFunctionName##Detour(); \
         private: 

#define  DIM_IMPL_STATIC_DETOUR(ClassName, FriendlyFunctionName, DetouredMethodType, DetouredMethodName, TrampolineName) \
         DetouredMethodType* ClassName::m_real##FriendlyFunctionName = NULL; \
         DetouredMethodType* ClassName::m_tramp##FriendlyFunctionName = NULL; \
         void ClassName::Install##FriendlyFunctionName##Detour(HMODULE ModuleHandle) \
         { \
            if(m_tramp##FriendlyFunctionName == NULL) /**/ \
            { /**/ \
               std::cout << "Attempt Detour: " #FriendlyFunctionName " " << std::endl; /**/ \
               m_real##FriendlyFunctionName = (DetouredMethodType*)GetProcAddress(ModuleHandle, DetouredMethodName); /**/ \
               m_tramp##FriendlyFunctionName = m_real##FriendlyFunctionName; /**/ \
               std::cout << "real" #FriendlyFunctionName ": " << m_real##FriendlyFunctionName << std::endl; /**/ \
               if (DetourTransactionBegin() != NO_ERROR) { \
                  std::cout << "DetourTransactionBegin failed with error " << GetLastError() << std::endl; \
               } \
               if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) { \
                  std::cout << "DetourUpdateThread failed with error " << GetLastError() << std::endl; \
               } \
               if (DetourAttach(&(PVOID&)m_tramp##FriendlyFunctionName, (PVOID)TrampolineName) != NO_ERROR) { \
                  std::cout << "DetourAttach failed with error " << GetLastError() << std::endl; \
               } \
               if (DetourTransactionCommit() != NO_ERROR) { \
                  std::cout << "DetourTransactionBegin failed with error " << GetLastError() << std::endl; \
               } \
               std::cout << "Done! Tramp: " << m_tramp##FriendlyFunctionName << std::endl; /**/ \
            }else{ /**/ \
               std::cout << "Already detoured " #FriendlyFunctionName "!: " << m_real##FriendlyFunctionName << std::endl; /**/ \
            } /**/ \
         } \
         void ClassName::Uninstall##FriendlyFunctionName##Detour() \
         { \
            if(m_tramp##FriendlyFunctionName != NULL) \
            { \
               std::cout << "Attempt undetour: " << m_real##FriendlyFunctionName << "  " << m_tramp##FriendlyFunctionName << std::endl; \
               if (DetourTransactionBegin() != NO_ERROR) { \
                  std::cout << "DetourTransactionBegin failed with error " << GetLastError() << std::endl; \
               } \
               if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) { \
                  std::cout << "DetourUpdateThread failed with error " << GetLastError() << std::endl; \
               } \
               if (DetourDetach(&(PVOID&)m_tramp##FriendlyFunctionName, (PVOID)TrampolineName) != NO_ERROR) { \
                  std::cout << "DetourAttach failed with error " << GetLastError() << std::endl; \
               } \
               if (DetourTransactionCommit() != NO_ERROR) { \
                  std::cout << "DetourTransactionBegin failed with error " << GetLastError() << std::endl; \
               } \
               std::cout << "Done!" << std::endl; /**/ \
            } \
         }