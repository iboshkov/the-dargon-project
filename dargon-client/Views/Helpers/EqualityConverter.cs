﻿using System;
using System.Globalization;
using System.Windows.Data;

namespace Dargon.Client.Views.Helpers {
   public class EqualityConverter : IMultiValueConverter {
      public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture) {
         return values[0].Equals(values[1]);
      }

      public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) {
         return null;
      }
   }
}
