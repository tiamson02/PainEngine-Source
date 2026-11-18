// This is simply a compilation test file. 



#include <stdio.h>

class CParticleDemo
{
//Operations:
public:
	CParticleDemo();
	virtual ~CParticleDemo()	{};

	virtual int  	Update() = 0;

private:
	virtual void 	CreateParticleArray() = 0;
		
public:
		char						m_titleText[128];
};

CParticleDemo::CParticleDemo()
{
	sprintf(m_titleText, "Untitled");
}

class CParticleDemo0 : public CParticleDemo
{
//Operations:
public:
	CParticleDemo0();
	virtual int  Update();


private:
	virtual void CreateParticleArray();

};


CParticleDemo0::CParticleDemo0() : CParticleDemo()
{
	sprintf(m_titleText, "Non-sorted particle array (unoptimised)");
}



void CParticleDemo0::CreateParticleArray()
{
	printf("test cpp");
}


int CParticleDemo0::Update()
{
	int	i;

	printf("test cpp");
	
	return 0;
}


int hellocpp()
{
	printf("Hello Gondor!");
	return 4;
}
