//! class for a ref counting resource holding thingie

using System;
using System.Collections;

namespace rho
{

    public abstract class ResourceController
    {
        ArrayList resources;
	
        class Resource
        {
            public object o;		//!< the thingie we're holding onto
            public int refcount;	//!< number of thingies using it
            public string fname;	//!< filename
		
            public Resource(object obj, string n)
            {
                o=obj;
                fname=n;
                refcount=1;
            }
        }
	
        public abstract object Load(string fname);

        public object this[string fname]
        {
            get
            {
                foreach (Resource r in resources)
                    if (r.fname==fname)
                    {
                        r.refcount++;
                        return r.o;
                    }
	
                object obj=Load(fname);
                resources.Add(new Resource(obj, fname));
			
                return obj;
            }
        }

        public void Free(object o)
        {
            foreach (Resource r in resources)
                if (r.o==o)
                {
                    r.refcount--;
                    if (r.refcount==0)
                    {
                        ((IDisposable)o).Dispose();
                        resources.Remove(r);	// now the GCer can clean up at its leisure.
                    }
				
                    return;
                }
        }
	
        public ResourceController()
        {
            resources=new ArrayList();
        }
    }

}