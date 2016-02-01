// TradeApiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// tradeapitest.cpp :
// һ���򵥵����ӣ�����CThostFtdcMdApi��CThostFtdcMdSpi�ӿڵ�ʹ�á�
// ��������ʾһ������¼������Ĺ���
#include <stdio.h>
#include <windows.h>
#include "ctp/ThostFtdcMdApi.h"

// ����¼������Ƿ���ɵı�־
// Create a manual reset event with no signal
HANDLE g_hEvent = CreateEvent(NULL, true, false, NULL);
// ��Ա����
TThostFtdcBrokerIDType g_chBrokerID;
// �����û�����
TThostFtdcUserIDType g_chUserID;
class CSimpleHandler : public CThostFtdcMdSpi
{
public:
	// ���캯������Ҫһ����Ч��ָ��CThostFtdcMdApiʵ����ָ��
	CSimpleHandler(CThostFtdcMdApi *pUserApi) : m_pUserApi(pUserApi) {}
	~CSimpleHandler() {}
	// ���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	virtual void OnFrontConnected()
	{
		CThostFtdcReqUserLoginField reqUserLogin;
		// get BrokerID
		printf("BrokerID:");
		scanf_s("%s", &g_chBrokerID, sizeof(TThostFtdcBrokerIDType) );
		strcpy_s(reqUserLogin.BrokerID, g_chBrokerID);
		// get userid
		printf("userid:");
		scanf_s("%s", &g_chUserID, sizeof(g_chUserID));
		strcpy_s(reqUserLogin.UserID, g_chUserID);
		// get password
		printf("password:");
		scanf_s("%s", &reqUserLogin.Password, sizeof(reqUserLogin.Password));
		// ������½����
		m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
	}
	// ���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
	virtual void OnFrontDisconnected(int nReason)
	{
		// ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
		printf("OnFrontDisconnected.\n");
	}
	// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspUserLogin:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		if (pRspInfo->ErrorID != 0) {
			// �˵�ʧ�ܣ��ͻ�������д�����
			printf("Failed to login, errorcode=%d errormsg=%s requestid=%d \
				chain = %d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
				exit(-1);
		}
		// �˵ǳɹ�
		// ��������
		char * Instrumnet[] = { "IF0809", "IF0812" };
		m_pUserApi->SubscribeMarketData(Instrumnet, 2);
		//���˶�����
		m_pUserApi->UnSubscribeMarketData(Instrumnet, 2);
	}
	// ����Ӧ��
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField
		*pDepthMarketData)
	{
		// �������¼����
		// printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,	pRspInfo->ErrorMsg);
		//�յ��������
		SetEvent(g_hEvent);
	};
	// ����û�����ĳ���֪ͨ
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast) {
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// �ͻ�������д�����
		// {�ͻ��˵Ĵ�����}
		
	}
private:
	// ָ��CThostFtdcMdApiʵ����ָ��
	CThostFtdcMdApi *m_pUserApi;
};
int main()
{
	// ����һ��CThostFtdcMdApiʵ��
	CThostFtdcMdApi *pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
	// ����һ���¼������ʵ��
	CSimpleHandler sh(pUserApi);
	// ע��һ�¼������ʵ��
	pUserApi->RegisterSpi(&sh);
	// ���ý����й�ϵͳ����ĵ�ַ������ע������ַ����
	pUserApi->RegisterFront("tcp://172.16.0.31:57205");
	// ʹ�ͻ��˿�ʼ���̨����������
	pUserApi->Init();
	// �ͻ��˵ȴ������������
	WaitForSingleObject(g_hEvent, INFINITE);
	// �ͷ�APIʵ��
	pUserApi->Release();

	getchar();

	return 0;
}