﻿namespace Dargon.LeagueOfLegends.Modifications
{
   public interface IManagableTask
   {
      void SetNext(ITask next);

      void UpdateProgress(Status status, int filesResolved, int filesUnresolved, int fileCount);
      void UpdateCompleted();
      void Cancel();
   }
}