#include "server.h"
#include "blowfish.h"

//returns nonce
//Tans function, I beleive.
long unsigned int generateNonce(){
	long unsigned int random = (rand() % ULONG_MAX) + 1;
#ifdef DEBUG
	cout << "Random Nonce: " << random << endl;
#endif
	return (random);
}

//Reads the information in from ini file.
void parseIni(){
	boost::property_tree::ptree config_tree;
	boost::property_tree::ini_parser::read_ini("server.ini", config_tree);

	num_clients = (config_tree.get<int>("system.num_clients"));
	kdc_address = (config_tree.get<string>("system.kdc_address"));
	kdc_port = (config_tree.get<string>("system.kdc_port"));

	for (int c = 0; c < num_clients; c++) {
		clientList[c] = config_tree.get<string>(boost::lexical_cast<std::string>(c)+".name");
		cout << "Added client: " << clientList[c] << "." << endl;
	}
}

//BLOWFISH
string encrypt(string key, string msg) {
	//encrypt msg using given key and return encrypted string

	return ("Encrypted:" + msg);
}
string decrypt(string key, string msg) {
	//decrypt msg using given key and return unencrypted string

	return (msg.erase(0,10));
}
//END BLOWFISH

void talk(int choice){
	try {
		boost::asio::io_service io_service;

		//get ip from hostname (of kdc)
		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), kdc_address, kdc_port);
		udp::endpoint receiver_endpoint = *resolver.resolve(query);

		//open socket
#ifdef DEBUG
		cout << "Socket open." << endl;
#endif
		udp::socket socket(io_service);
		socket.open(udp::v4());

		//say hello to kdc '0', i believe
#ifdef DEBUG
		cout << "sending to: " << receiver_endpoint.address()
			<< ":" << receiver_endpoint.port() << endl;
#endif

		//send message ("B name";"nonce";)
		string message = (clientList[choice] + ";" + boost::lexical_cast<std::string>(generateNonce()) + ";");
		socket.send_to(boost::asio::buffer(message, message.length()), receiver_endpoint);

		//recieve a response (
#ifdef DEBUG
		cout << "Waiting..." << endl;
#endif
		boost::array<char, 512> recv_buf;
		udp::endpoint sender_endpoint;
		size_t len = socket.receive_from(
			boost::asio::buffer(recv_buf), sender_endpoint);

#ifdef DEBUG
		std::cout.write(recv_buf.data(), len);
#endif
		//recv_buf.data() is...
		//-> SessionKey;ip_B;nonce1;(B-key-encrypted[sessionKey,ip_A]);
		//convert recieved char[] to a string
		string msg(recv_buf.begin(), recv_buf.begin() + (len - 1));
		//msg is now ; seperated, with length in len
		//msg is encrypted with our key.
		//this part is dumb, key should be in the ini file, which will be slightly odd
		//	as they all use the same ini file, which they would not do in reality.
		cout << "Key for this client is?";
		string myKey;
		cin >> myKey;
		msg = decrypt(myKey, msg);
		vector<string> info;
		boost::split(info, msg, boost::is_any_of("[; ]"));
		//info is now:
		//		0		1		2				3
		//->SessionKey;ip_B;nonce1;(B-key-encrypted[sessionKey,ip_A]);
#ifdef DEBUG
			cout << "send: " << info[3] << " to " << info[1] << endl;
#endif
			udp::resolver::query query2(udp::v4(), info[1], kdc_port);
			//boost::asio::ip::udp::endpoint dest( info[1], atoi(kdc_port.c_str()));
			udp::endpoint endpoint_B = *resolver.resolve(query2);
			socket.send_to(boost::asio::buffer(info[3],info[3].length()), endpoint_B);
			//recv Ks-encrypted nonce
#ifdef DEBUG
			cout << "Waiting..." << endl;
#endif
			len = socket.receive_from(
				boost::asio::buffer(recv_buf), endpoint_B);

			//check ... something and send back encrypted result
#ifdef DEBUG
			std::cout.write(recv_buf.data(), len);
			cout << "send: " << info[3] << " to " << info[1] << endl;
#endif
			string str2(recv_buf.begin(), recv_buf.begin() + (len - 1));
			msg = decrypt(info[0], str2);  //decrypt str2 w/ sessionkey
			//do something nonse related.
			socket.send_to(boost::asio::buffer(msg,msg.length()), endpoint_B);
			//CONNECTED!
	}
	catch (exception& e) {
		cerr << e.what() << endl;
	}
}

void waitForConnect(){
	try {
		boost::asio::io_service io_service;
		//open socket
#ifdef DEBUG
		cout << "Socket open." << endl;
#endif
		udp::socket socket(io_service, udp::endpoint(udp::v4(), atoi(kdc_port.c_str())));
		
#ifdef DEBUG
		cout << "Waiting..." << endl;
#endif
		boost::array<char, 512> recv_buf;
		udp::endpoint remote_endpoint;
		size_t len = socket.receive_from(
			boost::asio::buffer(recv_buf), remote_endpoint);

#ifdef DEBUG
		string clientIP = remote_endpoint.address().to_string();
		cout << "Recieved msg from: " << clientIP << endl;
		std::cout.write(recv_buf.data(), len - 1);
#endif
		//gen nonce2, encrypt with sessionKey and send back
		//convert recieved char[] to a string
		string msg(recv_buf.begin(), recv_buf.begin() + (len - 1));
		string sessionKey = decrypt("myKey", msg);
		msg = encrypt(sessionKey, boost::lexical_cast<std::string>(generateNonce()));
		//boost::array<char, 512> send_buf;
		socket.send_to(
			boost::asio::buffer( msg,msg.length() ), remote_endpoint);
		//recv sessionkey encrypted nonce2
#ifdef DEBUG
		cout << "Waiting..." << endl;
#endif
		len = socket.receive_from(
			boost::asio::buffer(recv_buf), remote_endpoint);
#ifdef DEBUG
		clientIP = remote_endpoint.address().to_string();
		cout << "Recieved msg from: " << clientIP << endl;
		std::cout.write(recv_buf.data(), len - 1);
#endif
		//CONNECTED!
	}
	catch (exception& e) {
		cerr << e.what() << endl;
	}
}

/*
int main(int argc, char* argv[]) {
	//read in serverinfo from file
	parseIni();
	
	bool valid = false;
	string input;
	int choice = 0;
	while (true){
		//display menu
		cout << "Choose B..." << endl;
		for (int c = 0; c < num_clients; c++){
			cout << c << ": " << clientList[c] << endl;
		}
		cout << num_clients << ": Wait for connections." << endl;
		//get input
		getline(cin, input);
		stringstream myStream(input);
		if ((myStream >> choice)){
			//we know know input is an int
			//valid input (choice) is an int: [0,num_Clients]
			if (choice == num_clients){
				waitForConnect();
				break;
			}
			else if ((choice >= 0) && (choice <= num_clients)) {
				talk(choice);
				break;
			}
		}
		//not valid
		cin.clear();
		cout << "\f" << "Invalid choice, choose again." << endl;
	}
	
	

	//this does nothing except keep the cmd window from closing automatically.
	char wait;
	cin >> wait;
	return 1;

}
*/

#define DWORD  		unsigned long
#define WORD  		unsigned short
#define BYTE  		unsigned char

int main(int argc, char* argv[]) {
	cout << "TEST BEGIN." << endl;
	
	CBlowFish BF;
	//init (BYTE[] key, len)
	BYTE key[] = "kais1111";
	BF.Initialize(key, 8);

	BYTE in[1024];
	BYTE out[1024];

	DWORD outlen = 0;
	string testmsg = "The Quick Brown Fox Jumped Over The Lazy Dog.";
	strcpy((char*)in, testmsg.c_str());

	cout << "IN(" << testmsg.length() << "):" << endl << in << endl;
	outlen = BF.GetOutputLength(BF.Encode(in,out,testmsg.length()));
	cout << "Encrypted(" << outlen << "):" << endl << out << endl;
	BF.Decode(out, out, outlen);
	cout << "Decrypted:" << endl << out << endl;


	cout << "TEST END." << endl;
	char wait;
	cin >> wait;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  