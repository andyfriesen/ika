//#define TEST_DICTGRID

using System;
using System.Collections;
using System.Collections.Specialized;
using System.Data;
using System.Windows.Forms;

#if TEST_DICTGRID
using System.Diagnostics;
#endif

namespace rho.Controls {
    public delegate void DictionaryValueAddedHandler(string name, string value);
    public delegate void DictionaryValueChangeHandler(string oldName, string name, string value);
    public delegate void DictionaryValueDeleteHandler(string name);

    /// <summary>
    /// DataGrid which displays key/value strings
    /// </summary>
    public class DictionaryGrid : DataGrid {
        public DictionaryGrid() {
            DataColumn nameCol = table.Columns.Add("Name", typeof(string));
            DataColumn valueCol = table.Columns.Add("Value", typeof(string));
            table.PrimaryKey = new DataColumn[] { nameCol };
            
            DataSource = table;

            table.ColumnChanging += new DataColumnChangeEventHandler(ColumnChanging);
            table.RowChanged += new DataRowChangeEventHandler(RowChanged);
        }

        public StringDictionary Values {
            get {
                return values;
            }
            set {
                values.Clear();
                table.Rows.Clear();
                foreach (DictionaryEntry e in value) {
                    string key = (string)e.Key;
                    string value_ = (string)e.Value;
                    values.Add(key, value_);
                    table.Rows.Add(new object[] { key, value_ } );
                }
            }
        }

        public event DictionaryValueAddedHandler ValueAdded;
        public event DictionaryValueChangeHandler ValueChanged;
        public event DictionaryValueDeleteHandler ValueDeleted;

        protected virtual void OnValueAdded(string name, string value) {
            values[name] = value;
            if (ValueAdded != null) {
                ValueAdded(name, value);
            }
        }

        protected virtual void OnValueChanged(string oldName, string name, string value) {
            if (oldName != name) {
                values.Remove(oldName);
            }
            values[name] = value;

            if (ValueChanged != null) {
                ValueChanged(oldName, name, value);
            }
        }

        protected virtual void OnValueDeleted(string name) {
            values.Remove(name);

            if (ValueDeleted != null) {
                ValueDeleted(name);
            }
        }

        void ColumnChanging(object sender, DataColumnChangeEventArgs e) {
            /**
             *  We remember the name of the row we are editing here so that
             *  we know which row was renamed.
             */
            oldName = e.Row["Name"] as string;
        }
        
        void RowChanged(object sender, DataRowChangeEventArgs e) {
            string name = (string)e.Row["Name"];
            string value = (string)e.Row["Value"];

            switch (e.Action) {
                case DataRowAction.Add:
                    OnValueAdded(name, value);
                    break;
                case DataRowAction.Change:
                    OnValueChanged(oldName, name, value);
                    break;
                case DataRowAction.Delete:
                    OnValueDeleted(name);
                    break;
                default:
                    System.Diagnostics.Debug.Assert(false, string.Format("Dunno how to handle {0}", e.Action));
                    break;
            }
        }
        
        string oldName; // HACK: need to store the old name someplace to handle name changes
        readonly DataTable table = new DataTable("Table");
        readonly StringDictionary values = new StringDictionary();

#if TEST_DICTGRID
        class Blah {

            public void RowAdded(string name, string value) {
                Console.WriteLine("Row added: {0} = {1}", name, value);
                Debug.Assert(!dict.ContainsKey(name));
                dict[name] = value;
            }

            public void RowRemoved(string name) {
                Console.WriteLine("Row removed: {0}", name);
                Debug.Assert(dict.ContainsKey(name));
            }

            public void RowChanged(string oldName, string name, string value) {
                Console.WriteLine("Row changed: {0} {1} {2}", oldName, name, value);
                Debug.Assert(dict.ContainsKey(oldName));
                if (oldName != name) {
                    dict.Remove(oldName);
                }
                dict[name] = value;
            }

            public readonly StringDictionary dict = new StringDictionary();
        }

        public static void Main(string[] args) {
            Form f = new Form();
            DictionaryGrid grid = new DictionaryGrid();
            grid.Dock = DockStyle.Fill;
            f.Controls.Add(grid);

            Blah blah = new Blah();
            grid.ValueAdded += new DictionaryValueAddedHandler(blah.RowAdded);
            grid.ValueChanged += new DictionaryValueChangeHandler(blah.RowChanged);
            grid.ValueDeleted += new DictionaryValueDeleteHandler(blah.RowRemoved);

            Application.Run(f);

            Console.WriteLine("this.values");
            Console.WriteLine("-----------");
            foreach (DictionaryEntry entry in grid.values) {
                Console.WriteLine("{0} => {1}", entry.Key, entry.Value);
            }

            Console.WriteLine("blah.dict");
            Console.WriteLine("---------");
            foreach (DictionaryEntry entry in blah.dict) {
                Console.WriteLine("{0} => {1}", entry.Key, entry.Value);
            }
        }
#endif
    }
}
