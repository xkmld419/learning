/*VER: 1*/ 
# ifndef RELOAD_USER_INFO_H
# define RELOAD_USER_INFO_H

#include "UserInfoCtl.h"
#include "Process.h"

class ReloadUserInfo : public UserInfoAdmin
{
  public:
	ReloadUserInfo();

  public:
	void reloadServState();
	void reloadCustPlan();
	void reloadServGroup();
	void reloadOfferInstance();
	void reloadServAttr();
	void reloadServLocation();
	void reloadServIdent();
	void reloadServAcct();
	void reloadServProduct();
	void reloadServ();

};


class ReloadUserInfoMgr : public Process
{
  public:
	ReloadUserInfoMgr();

  public:
	int run();

  private:
	void prepare();

  private:
	bool m_bServState;
	bool m_bCustPlan;
	bool m_bServGroup;
	bool m_bOfferInstance;
	bool m_bServAttr;
	bool m_bServLocation;
	bool m_bServIdent;
	bool m_bServAcct;
	bool m_bServProduct;
	bool m_bServ;

};

# endif
