﻿using Dargon.Daemon;
using Dargon.Game;
using Dargon.IO.RADS;
using Dargon.LeagueOfLegends.FileSystem;
using Dargon.LeagueOfLegends.Lifecycle;
using Dargon.LeagueOfLegends.Modifications;
using Dargon.LeagueOfLegends.Processes;
using Dargon.LeagueOfLegends.RADS;
using Dargon.LeagueOfLegends.Session;
using Dargon.ModificationRepositories;
using Dargon.Modifications;
using Dargon.Processes.Watching;
using NLog;

namespace Dargon.LeagueOfLegends
{
   public class LeagueGameServiceImpl : IGameHandler
   {
      private static readonly Logger logger = LogManager.GetCurrentClassLogger();

      private readonly LeagueConfiguration configuration = new LeagueConfiguration();
      private readonly DaemonService daemonService;
      private readonly ProcessWatcherService processWatcherService;
      private readonly ModificationRepositoryService modificationRepositoryService;
      private readonly ModificationImportService modificationImportService;
      private readonly RadsServiceImpl radsService;
      private readonly LeagueModificationRepositoryService leagueModificationRepositoryService;
      private readonly LeagueModificationResolutionService leagueModificationResolutionService;
      private readonly LeagueModificationCompilationService leagueModificationCompilationService;
      private readonly LeagueProcessWatcherServiceImpl leagueProcessWatcherService;
      private readonly LeagueSessionWatcherServiceImpl leagueSessionWatcherService;
      private readonly RiotFileSystem gameFileSystem;
      private readonly LeagueLifecycleService leagueLifecycleService;

      public LeagueGameServiceImpl(DaemonService daemonService, ProcessWatcherService processWatcherService, ModificationRepositoryService modificationRepositoryService, ModificationImportService modificationImportService)
      {
         logger.Info("Initializing League Game Service");
         this.daemonService = daemonService;
         this.processWatcherService = processWatcherService;
         this.modificationRepositoryService = modificationRepositoryService;
         this.modificationImportService = modificationImportService;

         this.radsService = new RadsServiceImpl(configuration.RadsPath);
         this.leagueModificationRepositoryService = new LeagueModificationRepositoryServiceImpl(modificationRepositoryService);
         this.leagueModificationResolutionService = new LeagueModificationResolutionServiceImpl(daemonService, radsService);
         this.leagueModificationCompilationService = new LeagueModificationCompilationServiceImpl(daemonService);
         this.leagueProcessWatcherService = new LeagueProcessWatcherServiceImpl(processWatcherService);
         this.leagueSessionWatcherService = new LeagueSessionWatcherServiceImpl(leagueProcessWatcherService);
         this.gameFileSystem = new RiotFileSystem(radsService, RiotProjectType.GameClient);
         this.leagueLifecycleService = new LeagueLifecycleServiceImpl(leagueModificationRepositoryService, leagueModificationResolutionService, leagueModificationCompilationService, leagueSessionWatcherService, radsService);

         RunDebugActions();
      }

      private void RunDebugActions()
      {
         modificationRepositoryService.ClearModifications();
         var mod = modificationImportService.ImportLegacyModification(
            GameType.LeagueOfLegends,
            @"C:\lolmodprojects\Tencent Art Pack 8.74 Mini",
            new[] {
               @"C:\lolmodprojects\Tencent Art Pack 8.74 Mini\ArtPack\Client\Assets\Images\Champions\Ahri_Square_0.png",
               @"C:\lolmodprojects\Tencent Art Pack 8.74 Mini\ArtPack\Client\Assets\Images\Champions\Annie_Square_0.png",
               @"C:\lolmodprojects\Tencent Art Pack 8.74 Mini\ArtPack\Client\Characters\Annie\AnnieLoadScreen.dds"
            });
         modificationRepositoryService.AddModification(mod);
      }
   }

   public class LeagueConfiguration
   {
      public string RadsPath { get { return @"V:\Riot Games\League of Legends\RADS"; } }
   }
}
