/*
 * HttpRequest.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include "HttpClient.h"
#include "HttpRequestDefine.h"
#include <common/KThread.h>

#define MAX_RESPONED_BUFFER 4 * 1024

class HttpRequestRunnable;
class HttpRequest;
class IHttpRequestCallback {
public:
	virtual ~IHttpRequestCallback(){};
	virtual void onSuccess(long requestId, string url, const char* buf, int size) = 0;
	virtual void onFail(long requestId, string url) = 0;
	virtual void onReceiveBody(long requestId, string url, const char* buf, int size) = 0;
};

class HttpRequest : public IHttpClientCallback {
	friend class HttpRequestRunnable;
public:
	HttpRequest();
	virtual ~HttpRequest();

	// add by samson
	long StartRequest(const string& url, const HttpEntiy& entiy);
	long StartRequest(string host, string path, const HttpEntiy& entiy);
	void StopRequest(bool bWait = false);
	void SetCallback(IHttpRequestCallback *callback);
	// 设置是否缓存返回结果, 默认是缓存
	void SetNoCacheData(bool bCache = true);

	string GetUrl() const;
	string GetHost() const;
	string GetPath() const;
	long GetRequestId() const;

	// 获取下载总数据量及已收数据字节数
	void GetRecvDataCount(int& total, int& recv) const;
	// 获取上传总数据量及已收数据字节数
	void GetSendDataCount(int& total, int& send) const;
	// 获取 Content-Type
	string GetContentType() const;

protected:
	void onReceiveBody(HttpClient* client, const char* buf, int size);

private:
	void InitRespondBuffer();
	bool AddRespondBuffer(const char* buf, int size);

	HttpClient mHttpClient;
	KThread mKThread;
	HttpRequestRunnable* mpHttpRequestRunnable;
	IHttpRequestCallback* mpIHttpRequestCallback;

	string mUrl;
	string mHost;
	string mPath;
	HttpEntiy mEntiy;

	char* mpRespondBuffer;
	int miCurrentSize;
	int miCurrentCapacity;
	bool mbCache;
};

#endif /* HTTPREQUEST_H_ */
