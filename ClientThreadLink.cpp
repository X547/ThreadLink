#include "ClientThreadLink.h"
#include <PthreadMutexLocker.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <new>


ClientThreadLink::ClientThreadLink(ClientThreadLinkConnection *conn, const BMessenger &serverMsgr):
	fConn(conn), fPort(create_port(100, "client"))
{
	fprintf(stderr, "+ThreadLink(), thread: %" B_PRId32 "\n", find_thread(NULL));
	fLink.SetTo(-1, fPort.Get());
	int32 replyCode;
	port_id serverThreadPort;
	BMessage msg(connectMsg);
	msg.AddInt32("port", fPort.Get());
	serverMsgr.SendMessage(&msg);
	fLink.GetNextMessage(replyCode);
	fLink.Read<int32>(&serverThreadPort);
	fLink.SetTo(serverThreadPort, fPort.Get());
}

ClientThreadLink::~ClientThreadLink()
{
	fprintf(stderr, "-ThreadLink(), thread: %" B_PRId32 "\n", find_thread(NULL));
	fLink.StartMessage(disconnectMsg);
	fLink.Flush();
	PthreadMutexLocker lock(&fConn->fLock);
	fConn->fLinks.Remove(this);
}


ClientThreadLinkConnection::ClientThreadLinkConnection()
{
	if (pthread_key_create(&fLinkTls, [](void *arg) {
		delete static_cast<ClientThreadLink*>(arg);
	}) < 0) abort();
}

ClientThreadLinkConnection::~ClientThreadLinkConnection()
{
	PthreadMutexLocker lock(&fLock);
	while (ClientThreadLink *threadLink = fLinks.First()) {
		delete threadLink;
	}
	if (pthread_key_delete(fLinkTls) < 0) abort();
}

void ClientThreadLinkConnection::SetMessenger(const BMessenger &serverMsgr)
{
	fServerMsgr = serverMsgr;
}

ClientThreadLink *ClientThreadLinkConnection::GetLink()
{
	ClientThreadLink *threadLink = (ClientThreadLink*)pthread_getspecific(fLinkTls);
	if (threadLink != NULL) return threadLink;
	PthreadMutexLocker lock(&fLock);
	threadLink = new(std::nothrow) ClientThreadLink(this, fServerMsgr);
	if (threadLink == NULL) return NULL;
	fLinks.Insert(threadLink);
	pthread_setspecific(fLinkTls, threadLink);
	return threadLink;
}
