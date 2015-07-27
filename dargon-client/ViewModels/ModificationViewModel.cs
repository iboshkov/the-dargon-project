﻿using Dargon.Client.Annotations;
using Dargon.Modifications;
using ItzWarty;
using System;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using Dargon.LeagueOfLegends.Modifications;
using Dargon.Patcher;

namespace Dargon.Client.ViewModels {
   public class ModificationViewModel : INotifyPropertyChanged {
      private readonly Modification2 modification;
      public event PropertyChangedEventHandler PropertyChanged;

      public ModificationViewModel(Modification2 modification) {
         this.modification = modification;
      }

      public string RepositoryPath => modification.RepositoryPath;
      public string RepositoryName => modification.RepositoryName;
      public string Name { get { return modification.FriendlyName; } set { modification.FriendlyName = value; OnPropertyChanged(); } }
      public string[] Authors { get { return modification.Authors; } set { modification.Authors = value; OnPropertyChanged(); } }
      public string Author => Authors.Join(", ");
      public ModificationStatus Status { get { return ModificationStatus.Enabled; } set { throw new NotImplementedException(); } }
      public LeagueModificationCategory Category => LeagueModificationCategory.FromString(File.ReadAllText(new LocalRepository(RepositoryPath).GetMetadataFilePath("CATEGORY"), Encoding.UTF8));

      [NotifyPropertyChangedInvocator]
      protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null) {
         PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
      }
   }

   [Flags]
   public enum ModificationStatus {
      Enabled = 0x01,
      Disabled = 0x02,
      Broken = 0x04,
      UpdateAvailable = 0x08
   }
}
