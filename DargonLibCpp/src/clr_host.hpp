#pragma once
#include <string>
#include <vector>

struct ICLRMetaHost;
struct ICLRRuntimeInfo;
struct ICLRRuntimeHost;

namespace dargon {
   class clr_host {
      static ICLRMetaHost* metaHost;
      static ICLRRuntimeInfo* runtime;
      static ICLRRuntimeHost* runtimeHost;

   public:
      static void init(std::wstring version);
      static void load_assembly(std::wstring path, std::wstring arguments);
   };

   class clr_utilities {
   public:
      static std::vector<std::wstring> enumerate_runtime_versions();
      static std::wstring pick_runtime_version();
   };
}