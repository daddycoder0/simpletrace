#ifndef _PROGRESS_H_
#define _PROGRESS_H_

class Progress
{
	public:
		Progress(const char * name)
		{
			m_name = name;
			m_progress = 0.f;
			m_intProgress = -1;
		}

		void UpdateProgress(float progress)
		{
			m_progress = progress;

			int p = int(m_progress * 100);

			if (p != m_intProgress)
			{
				if (m_intProgress < 0)
				{
					cout << m_name << ":" << endl;
				}
				m_intProgress = p;
				cout << "\r[";
				unsigned int bW = 55;
				unsigned int actBarW = bW - 2;
				for (unsigned int i = 0; i< actBarW;i++)
				{
					cout << (((float)i / (float)actBarW < m_progress) ? "#" : " ");
				}

				cout << "] " << m_intProgress << "%";

				if (m_progress >= 1.f)
				{
					cout << endl << endl;
				}
				cout << flush;
			}
		}
	private:

		float m_progress;
		int m_intProgress;
		const char* m_name;

};
#endif