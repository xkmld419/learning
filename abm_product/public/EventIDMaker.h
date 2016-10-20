/*VER: 1*/ 
#ifndef EVENT_ID_MAKER_H
#define EVENT_ID_MAKER_H

class EventIDMaker 
{
  public:
    static long getNextEventID();
    static long getNextEventSeq();

  private:
    static long m_lFlag;
    static long m_lCurEventID;

};

#endif

