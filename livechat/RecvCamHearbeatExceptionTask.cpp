/*
 * author: Alex shum
 *   date: 2016-11-28
 *   file: RecvCamHearbeatExceptionTask.cpp
 *   desc: Cam聊天扣费心跳包通知（仅男士端） Task实现类 （有异常cam扣费心跳才接收）
 */

#include "RecvCamHearbeatExceptionTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/CommonFunc.h>
#include <common/KLog.h>

// 返回参数定义
// 异常描述
#define EXCEPTION_NAME_PARAMS  "exceptionName"
// 异常错误Code
#define EXCEPTION_CODE_PARAMS  "exceptionCode"
// Camshare对象Id
#define TARGETID_PARAMS        "targetId"

RecvCamHearbeatExceptionTask::RecvCamHearbeatExceptionTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_targetId = "";
}

RecvCamHearbeatExceptionTask::~RecvCamHearbeatExceptionTask(void)
{
}

// 初始化
bool RecvCamHearbeatExceptionTask::Init(ILiveChatClientListener* listener)
{
	bool result = false;
	if (NULL != listener)
	{
		m_listener = listener;
		result = true;
	}
	return result;
}
	
// 处理已接收数据
bool RecvCamHearbeatExceptionTask::Handle(const TransportProtocol* tp)
{
	
	bool result = false;

	FileLog("LiveChatClient", "RecvCamHearbeatExceptionTask::Handle() begin, tp->data:%p, tp->dataLen:%d, tp->data[0]:%d", tp->data, tp->GetDataLength(), tp->data[0]);
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		if(root->type == DT_OBJECT)
		{
			// exceptionName
			amf_object_handle exceptionNameObject = root->get_child(EXCEPTION_NAME_PARAMS);
			result = !exceptionNameObject.isnull() && exceptionNameObject->type == DT_STRING;
			if (result) {
				m_errMsg = exceptionNameObject->strValue;
			}

			// exceptionCode
			amf_object_handle exceptionCodeObject = root->get_child(EXCEPTION_CODE_PARAMS);
			result = !exceptionCodeObject.isnull() && exceptionCodeObject->type == DT_INTEGER ;
			if (result) {
				m_errType = (LCC_ERR_TYPE)exceptionCodeObject->intValue;
			}

			// targetId
			amf_object_handle targetIdObject = root->get_child(TARGETID_PARAMS);
			result = !targetIdObject.isnull() && targetIdObject->type == DT_STRING;
			if (result) {
				m_targetId = targetIdObject->strValue;
			}
		}
	}

	// 协议解析失败
	if (!result) {
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}

	FileLog("LiveChatClient", "RecvCamHearbeatExceptionTask::Handle() m_errType:%d", m_errType);

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnRecvCamHearbeatException(m_errMsg, m_errType, m_targetId);
		FileLog("LiveChatClient", "RecvCamHearbeatExceptionTask::Handle() callback end, result:%d", result);
	}
	
	FileLog("LiveChatClient", "RecvCamHearbeatExceptionTask::Handle() end");

	return result;
	
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvCamHearbeatExceptionTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvCamHearbeatExceptionTask::GetSendDataProtocolType()
{
	return AMF_PROTOCOL;
}
	
// 获取命令号
int RecvCamHearbeatExceptionTask::GetCmdCode()
{
	return TCMD_RECVCAMHEARBEATEXCEPTION;
}

// 设置seq
void RecvCamHearbeatExceptionTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvCamHearbeatExceptionTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvCamHearbeatExceptionTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvCamHearbeatExceptionTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvCamHearbeatExceptionTask::OnDisconnect()
{
	// 不用回调
}