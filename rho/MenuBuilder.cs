using System;
using System.Windows.Forms;

namespace rho {
    /// <summary>
    /// Summary description for MenuBuilder.
    /// </summary>
    public sealed class MenuBuilder {
        private MenuBuilder() {}

        public static MenuItem menu(string name, params MenuItem[] children) {
            return new MenuItem(name, children);
        }

        public static MenuItem menu(string name, EventHandler func) {
            return new MenuItem(name, func);
        }

        public static MenuItem menu(string name, EventHandler func, Shortcut shortcut) {
            return new MenuItem(name, func, shortcut);
        }

        public static MenuItem menu(int mergeOrder, string name, params MenuItem[] children) {
            return menu(mergeOrder, name, null, Shortcut.None, children);
        }

        public static MenuItem menu(int mergeOrder, string name) {
            return menu(mergeOrder, name, null, Shortcut.None, null);
        }

        public static MenuItem menu(int mergeOrder, string name, EventHandler func, Shortcut shortcut) {
            return menu(mergeOrder, name, func, shortcut, null);
        }

        public static MenuItem menu(int mergeOrder, string name, EventHandler func) {
            return menu(mergeOrder, name, func, Shortcut.None, null);
        }

        public static MenuItem menu(int mergeOrder, string name, EventHandler func, Shortcut shortcut, MenuItem[] children) {
            return new MenuItem(MenuMerge.MergeItems, mergeOrder, shortcut, name, func, null, null, children);
        }

        public static MenuItem separator() {
            return new MenuItem("-");
        }
    }
}
