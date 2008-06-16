#ifndef RSA_H_
#define RSA_H_

class RSA
{
	private:
		
	public:
		const std::string &Encrypt(const std::string input, const Key &key);
		const std::string &Decrypt(const std::string input, const Key &key);
		void GenerateKeyPair(Key &privateKey, Key &publicKey);
};

#endif /*RSA_H_*/
