﻿using Dargon.Management;
using Dargon.Modifications;
using Dargon.PortableObjects;
using Dargon.Processes;
using Dargon.Services.Messaging;

namespace Dargon {
   public class ClientPofContext : PofContext {
      public ClientPofContext() {
         // Dargon Service Protocol 1-999
         MergeContext(new DspPofContext());

         // Dargon Management 1000-1999
         MergeContext(new ManagementPofContext());

         // Hydar 2000-2999
         
         // Nest Exeggutor 3000-3999
         
         // Modification-Impl 10000-10999
         MergeContext(new ModificationPofContext());

         // Process-API and Impl 11000-11999
         MergeContext(new ProcessImplPofContext());

         // Wyvern Account-API 1000000-1000999
      }
   }
}