using System;
using System.Collections;
using System.Text;

namespace rho {
    /// <summary>
    /// Stupid little helper 'class' because manually building filter strings 
    /// for FileDialogs is a pain.
    /// </summary>
    public sealed class FilterBuilder {
        private FilterBuilder() {
        }

        public static string Build(params string[] args) {
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < args.Length; i += 2) {
                sb.AppendFormat("{0} ({1})|{1}|", args[i], args[i + 1]);
            }
            
            sb.Remove(sb.Length - 1, 1); // clip trailing pipe
            
            return sb.ToString();
        }
    }
}
