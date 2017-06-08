#ifndef _PROGRESS_H_
#define _PROGRESS_H_

class Progress
{
	public:
		Progress(const char * name)
		{
			m_name = name;
			m_progress = 0.f;
			cout << name << ":" << endl;
		}

		void UpdateProgress(float progress)
		{
			m_progress = progress;
			cout << "\r[";
			unsigned int bW = 55;
			unsigned int actBarW = bW - 2;
			for (unsigned int i=0; i< actBarW;i++)
			{
				cout << (((float)i / (float)actBarW < m_progress) ? "#" : " ");
			}
			
			cout << "] " << int(m_progress * 100) << "%";

			if (m_progress >= 1.f)
			{
				cout << endl << endl;
			}
			cout << flush;
		}
	private:

		float m_progress;
		const char* m_name;

};
#endif