/*
	Crummy little stack thingie.  v1.0
	coded by tSB Jan 14th, '01
*/

const int stacksize=1024;

template <class T>
class CStack
{
private:
	T m_data[stacksize];
	int m_size;
public:
	CStack()
	{
		m_size=0;
	}

	void Push(T dat)
	{
		if (m_size<stacksize)
			m_data[m_size++]=dat;
	}

	T Pop()
	{
		static T dummy;
		if (!m_size)
			return dummy;	// uh oh

		return m_data[--m_size];
	}

	void PushCount(int count)
	{
		m_size+=count;
		if (m_size>=stacksize)
			m_size=stacksize-1;
	}

	void PopCount(int count)
	{
		m_size-=count;
		if (m_size<0) m_size=0;
	}

	int  Position()
	{
		return m_size;
	}

	T& operator[](int idx)
	{
		static T dummy;

		if (idx<0 || idx>m_size)
			return dummy;
		return m_data[idx];
	}
};