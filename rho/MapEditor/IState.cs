using System;
using System.Reflection;
using System.Collections;

namespace rho.MapEditor
{
    using System.Windows.Forms;

	/// <summary>
	/// Represents the current state of the map editor.
	/// </summary>
	abstract class State
	{
        public abstract string Name {   get;    }

        public abstract void MouseDown(MapView view,MouseEventArgs e);
        public abstract void MouseUp(MapView view,MouseEventArgs e);
        public abstract void MouseWheel(MapView view,MouseEventArgs e);
        public abstract void MouseMove(MapView view,MouseEventArgs e);

        public abstract void KeyDown(MapView view,KeyEventArgs e);
        public abstract void KeyUp(MapView view,KeyEventArgs e);

        static State[] states;

        public static State[] States
        {
            get {   return states;  }
        }

        static State()
        {
            ArrayList list=new ArrayList();

            Type[] types=Assembly.GetExecutingAssembly().GetTypes();
            Type statetype=typeof(State);

            foreach (Type t in types)
            {
                if (statetype.IsAssignableFrom(t))
                {
                    ConstructorInfo c=t.GetConstructor(Type.EmptyTypes);
                    if (c!=null)
                        list.Add(c.Invoke(null));
                }
            }

            states=new State[list.Count];
            int i=0;
            foreach (State s in list)
                states[i++]=s;
        }
    }
}
