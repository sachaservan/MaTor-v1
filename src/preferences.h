#ifndef PREFERENCES_H
#define PREFERENCES_H

/*
 * This class is used to encapsulate all preferences that a user might have specified
 */
class Preferences
{
	public:
		// class constructor
		Preferences();
		Preferences(Preferences* prefs);
		// class destructor
		~Preferences();
		bool useGuards() const;
		bool exitNodes() const;
		bool allowNonValidEntry() const;
		bool allowNonValidMiddle() const;
		bool allowNonValidExit() const;
		bool fastCircuit() const;
		bool requireStable() const;
		void setUseGuards(bool flag){this->flagUseGuards = flag;}
		void setExitNodes(bool flag){this->flagExitNodes = flag;}
		void setAllowNonValidEntry(bool flag){this->flagAllowNonValidEntry = flag;}
		void setAllowNonValidMiddle(bool flag){this->flagAllowNonValidMiddle = flag;}
		void setAllowNonValidExit(bool flag){this->flagAllowNonValidExit = flag;}
		void setFastCircuit(bool flag){this->flagFastCircuit = flag;}
		void setRequireStable(bool flag){this->flagRequireStable = flag;}
	private:
		bool flagUseGuards;
		bool flagExitNodes;
		bool flagAllowNonValidEntry;
		bool flagAllowNonValidMiddle;
		bool flagAllowNonValidExit;
		bool flagFastCircuit;
		bool flagRequireStable;
		
};

#endif // PREFERENCES_H
