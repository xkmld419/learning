#include "platform.h"
#include "MessageQueue.h"
#include <string.h>
#include "Exception.h"

#include "platform.h"

int main(int argc, char *argv[])
{
	int i;
	char c;
	char buf[2048];
	int l;

	MessageQueue mq(10220);
	if (!mq)
	{
		cout << "Error1" << endl;
		return 0;
	}

	try
	{
		mq.open(false, true);
	}
	catch (Exception &e)
	{
		cout << e.descript() << endl;
		return 0;
	}
	catch (...)
	{
		cout << "xxx" << endl;
	}

	while (1)
	{
		cout << "1 Send Message\n2 Recv Message" << endl;
		cin >> i;
		if (i == 1)
		{
			l = 0;
			while (1)
			{
				cin >> c;
				if (c != '0')
				{
					buf[l] = c;
					l++;
				}
				else
				{
					buf[l] = 0;
					cout << "sending:" << buf << endl;
					try
					{
						if (mq.send(buf, l) != l)
							cout << "Error Send" << endl;
						else
							cout << "Success Send" << endl;
					}
					catch (Exception e)
					{
						cout << e.descript() << endl;
					}
					break;
				}
			}
		}
		else if (i == 2)
		{
			i = 1;
			cout << "Recv:" << endl;
			try
			{
				while ((l=mq.receive(buf)) > 0)
				{
					buf[l] = 0;
					cout << i << ":" << buf << endl;
					i++;
				}
			}
			catch (Exception e)
			{
				cout << e.descript() << endl;
			}
			cout << "End Recv" << endl;
		}
		else if (i == 3)
		{
			strcpy(buf, "abcdefgadsfsafdsdfsafsafsaf");
			buf[20] = 0;
			for (i=0 ; i<9999; i++)
			{
				try
				{
					mq.send(buf, 20);
				}
				catch (Exception e)
				{
					cout << e.descript() << endl;
				}
			}
		}
		else
		{
			cout << mq.getMessageNumber () << endl;
			break;
		}
	}

	try
	{
		mq.close();
	}
	catch (Exception e)
	{
		cout << e.descript() << endl;
		return 0;
	}


	cout << "Hello, world\n" << endl;
	
	return 0;
}

