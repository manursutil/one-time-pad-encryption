/*
 * One-Time Pad Encryption/Decryption Demo
 * WARNING: This is for educational purposes only!
 * - Uses XOR encryption with a random key (one-time pad principle)
 * - NOT cryptographically secure due to PRNG used
 * - Keys stored in plaintext
 * - No message authentication
 */

 #include <cstdint>
 #include <cstdlib>
 #include <fstream>
 #include <iostream>
 #include <random>
 #include <stdexcept>
 #include <string>
 #include <vector>
 
 enum class ErrorCode {
     SUCCESS = 0,
     FILE_ERROR = 1,
     PARSE_ERROR = 2,
     SIZE_MISMATCH = 3,
     USAGE_ERROR = 4
 };
 

 using KeyType = std::vector<uint8_t>;
 using EncryptedType = std::vector<uint8_t>;
 

 KeyType generate_key(std::size_t length)
 {

     std::random_device rd;
     std::mt19937 gen(rd());
     std::uniform_int_distribution<int> dis(0, 255);
     
     KeyType key;
     key.reserve(length);
     
     for (std::size_t i = 0; i < length; i++)
     {
         key.push_back(static_cast<uint8_t>(dis(gen)));
     }
     
     return key;
 }
 
 EncryptedType encrypt(const std::string& file_path, const std::string& key_output_path)
 {

     std::ifstream file(file_path);
     if (!file)
     {
         throw std::runtime_error("Unable to open input file: " + file_path);
     }
 

     std::string plain_text;
     std::string word;
     while (file >> word)
     {
         plain_text += word;
         plain_text += " ";
     }
     file.close();
     

     if (!plain_text.empty() && plain_text.back() == ' ')
     {
         plain_text.pop_back();
     }
 

     KeyType key = generate_key(plain_text.length());
 
     std::ofstream key_file(key_output_path);
     if (!key_file)
     {
         throw std::runtime_error("Unable to create key file: " + key_output_path);
     }
     
     for (std::size_t i = 0; i < key.size(); i++)
     {
         key_file << static_cast<int>(key[i]);
         if (i < key.size() - 1)
         {
             key_file << " ";
         }
     }
     key_file.close();
 

     EncryptedType encrypted;
     encrypted.reserve(plain_text.length());
     
     for (std::size_t i = 0; i < plain_text.length(); i++)
     {
         uint8_t xored = static_cast<uint8_t>(plain_text[i]) ^ key[i];
         encrypted.push_back(xored);
     }
 
     return encrypted;
 }
 
 std::string decrypt(const std::string& key_file_path, const std::string& encrypted_file_path)
 {

     std::ifstream key_file(key_file_path);
     if (!key_file)
     {
         throw std::runtime_error("Unable to open key file: " + key_file_path);
     }
 
     std::ifstream encrypted_file(encrypted_file_path);
     if (!encrypted_file)
     {
         throw std::runtime_error("Unable to open encrypted file: " + encrypted_file_path);
     }
 
     KeyType key;
     int key_num;
     while (key_file >> key_num)
     {
         if (key_num < 0 || key_num > 255)
         {
             throw std::out_of_range("Key value out of byte range: " + std::to_string(key_num));
         }
         key.push_back(static_cast<uint8_t>(key_num));
     }
 
     if (key_file.bad())
     {
         throw std::runtime_error("Error reading key file");
     }
 
     EncryptedType encrypted;
     int encrypted_num;
     while (encrypted_file >> encrypted_num)
     {
         if (encrypted_num < 0 || encrypted_num > 255)
         {
             throw std::out_of_range("Encrypted value out of byte range: " + std::to_string(encrypted_num));
         }
         encrypted.push_back(static_cast<uint8_t>(encrypted_num));
     }
 
     if (encrypted_file.bad())
     {
         throw std::runtime_error("Error reading encrypted file");
     }
 
     if (key.size() != encrypted.size())
     {
         std::cerr << "Warning: Key size (" << key.size() 
                   << ") and encrypted size (" << encrypted.size() 
                   << ") do not match. Using minimum size.\n";
     }
 
     std::string decrypted;
     std::size_t len = std::min(key.size(), encrypted.size());
     decrypted.reserve(len);
     
     for (std::size_t i = 0; i < len; i++)
     {
         char xored = static_cast<char>(encrypted[i] ^ key[i]);
         decrypted.push_back(xored);
     }
 
     return decrypted;
 }
 
 int main(int argc, char* argv[])
 {
     try
     {
         if (argc < 2)
         {
             std::cout << "Usage:\n";
             std::cout << "  " << argv[0] << " encrypt <input_text_file>\n";
             std::cout << "    - Encrypts <input_text_file> and outputs 'output.txt' and 'secret_key.txt'\n";
             std::cout << "  " << argv[0] << " decrypt <secret_key.txt> <output.txt>\n";
             std::cout << "    - Decrypts <output.txt> using <secret_key.txt> and prints the result\n";
             return static_cast<int>(ErrorCode::USAGE_ERROR);
         }
 
         std::string mode = argv[1];
 
         if (mode == "encrypt")
         {
             if (argc < 3) 
             {
                 std::cerr << "Error: Missing input file.\n";
                 std::cerr << "Usage: " << argv[0] << " encrypt <input_text_file>\n";
                 return static_cast<int>(ErrorCode::USAGE_ERROR);
             }
 
             std::string input_file = argv[2];
             std::string key_file = "secret_key.txt";
             std::string output_file = "output.txt";
 
             EncryptedType encrypted = encrypt(input_file, key_file);
 
             std::ofstream out(output_file);
             if (!out) 
             {
                 throw std::runtime_error("Unable to create output file: " + output_file);
             }
 
             for (std::size_t i = 0; i < encrypted.size(); i++)
             {
                 out << static_cast<int>(encrypted[i]);
                 if (i < encrypted.size() - 1)
                 {
                     out << " ";
                 }
             }
             out.close();
 
             std::cout << "Success! Created '" << output_file 
                       << "' and '" << key_file << "'\n";
         }
         else if (mode == "decrypt")
         {
             if (argc < 4) 
             {
                 std::cerr << "Error: Missing arguments.\n";
                 std::cerr << "Usage: " << argv[0] << " decrypt <secret_key.txt> <output.txt>\n";
                 return static_cast<int>(ErrorCode::USAGE_ERROR);
             }
 
             std::string key_file_path = argv[2];
             std::string encrypted_file_path = argv[3];
 
             std::string decrypted = decrypt(key_file_path, encrypted_file_path);
             std::cout << "Decrypted text:\n" << decrypted << "\n";
         }
         else
         {
             std::cerr << "Invalid mode '" << mode << "'. Use 'encrypt' or 'decrypt'.\n";
             return static_cast<int>(ErrorCode::USAGE_ERROR);
         }
 
         return static_cast<int>(ErrorCode::SUCCESS);
     }
     catch (const std::exception& e)
     {
         std::cerr << "Error: " << e.what() << "\n";
         return static_cast<int>(ErrorCode::FILE_ERROR);
     }
 }