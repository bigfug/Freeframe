#ifndef SYPHONSEND_H
#define SYPHONSEND_H

class SyphonSend
{
public:
	explicit SyphonSend( void );
	virtual ~SyphonSend( void );

	void start( const char *pServerName, void *pContext );
	void stop( void );

	bool hasClients( void );

	void publishFrame( int pTexId, int pTarget, int pRegW, int pRegH, int pTexW, int TexH, bool pFlipped );

private:
	void		*mServer;
};

#endif // SYPHONSEND_H
