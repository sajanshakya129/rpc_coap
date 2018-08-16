/*Client RPC- part of stub generation.
 It has three function to initiate connection, make a call and terminate connection*/

Class ClientRpc
{
public:
	ClientRPC();// ServiceName *Name
	//ClientRPC(const char* ServiceName =0);
	virtual ~ClientRPC();//ensures to invoke destructor

	void initialize( int argc, char *argv[]);
	void caller();//opcode, call, Int errcode, result
	virtual void terminator(); 
}