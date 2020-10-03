#pragma once

enum Event {Start, Update, Destroy};

class Updateable
{
	public:
		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}
		void PublishEvent(Event event)
		{
			switch (event)
			{
				case Start:
					OnStart();
					break;
				case Update:
					OnUpdate();
					break;
				case Destroy:
					OnDestroy();
					break;
			}
		}
};