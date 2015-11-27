﻿using Dargon.Daemon;
using System;
using System.Threading;
using System.Windows.Forms;
using NLog;

namespace Dargon.Tray
{
   public class TrayServiceImpl : TrayService {
      private static readonly Logger logger = LogManager.GetCurrentClassLogger();
      private readonly DaemonService daemonService;
      private NotifyIcon notifyIcon;

      public TrayServiceImpl(DaemonService daemonService) {
         this.daemonService = daemonService;
      }

      public void Initialize() {
         logger.Info("Initialize tray service.");
         new Thread(TrayServiceThreadStart) { IsBackground = true }.Start();
      }

      private void TrayServiceThreadStart()
      {
         notifyIcon = new NotifyIcon();
         notifyIcon.Icon = CommonResources.Resources2011.Icon;
         notifyIcon.Visible = true;

         var menu = new ContextMenu();

         menu.MenuItems.Add(
            "Force GC",
            (s, e) => GC.Collect(3, GCCollectionMode.Forced));

         menu.MenuItems.Add(
            "Quit",
            (s, e) => daemonService.Shutdown());

         notifyIcon.ContextMenu = menu;

         daemonService.ShuttingDown += (sender, args) => {
            notifyIcon.Visible = false;
            notifyIcon.Dispose();
         };
         Application.Run();
      }
   }
}
