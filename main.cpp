#include <cstdint>
#include <cstddef>
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
    std::ifstream file;
    file.open(file_path);
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

std::string decrypt(std::string key_file_path, std::string encrypted_file_path)
{
    std::ifstream key_file;
    key_file.open(key_file_path);

    std::ifstream encrypted_file;
    encrypted_file.open(encrypted_file_path);

    if (!key_file || !encrypted_file)
    {
        std::cout << "Unable to open file\n";
        std::exit(1);
    }

    std::vector<int> key;
    std::string key_num_string;
    while (key_file >> key_num_string)
    {
        try 
        {
            key.push_back(std::stoi(key_num_string));
        }
        catch (...)
        {
            std::cout << "Error parsing key file data.\n";
            std::exit(1);
        }
    }

    std::vector<int> encrypted;
    std::string num_string;
    while (encrypted_file >> num_string)
    {
        try 
        {
            encrypted.push_back(std::stoi(num_string));
        }
        catch (const std::out_of_range& e)
        {
            std::cout << "Error: a number in the encrypted file is too large: " << num_string << "\n";
            std::exit(1);
        }
        catch (const std::invalid_argument& e)
        {
            std::cout << "Error: encypted file contains non-number data: " << num_string << "\n";
            std::exit(1);
        }
    }

    if (key.size() != encrypted.size())
    {
        std::cout << "Warning: Key size (" << key.size() << ") and Encrypted size (" << encrypted.size() << ") do not match.\n";
    }

    std::vector<int> decrypted;
    size_t len = std::min(key.size(), encrypted.size());

    for (std::size_t i = 0; i < len; i++)
    {
        char xored = encrypted[i] ^ key[i];
        decrypted.push_back(xored);
    }

    return std::string(decrypted.begin(), decrypted.end());
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    if (argc < 2)
    {
        std::cout << "Usage:\n";
        std::cout << "  " << argv[0] << " encrypt <input_text_file>\n";
        std::cout << "    - Encrypts <input_text_file> and outputs 'output.txt' and 'secret_key.txt'\n";
        std::cout << "  " << argv[0] << " decrypt <secret_key.txt> <output.txt>\n";
        std::cout << "    - Decrypts <output.txt> using <secret_key.txt> and prints the result\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "encrypt")
    {
        if (argc < 3) 
        {
            std::cout << "Error: Missing input file.\nUsage: " << argv[0] << " encrypt <input_text_file>\n";
            return 1;
        }

        std::vector<int> encrypted = encrypt(argv[2]);

        std::ofstream output_file("output.txt");
        if (!output_file) 
        {
            std::cout << "Unable to create output.txt\n";
            return 1;
        }

        for (int num : encrypted)
        {
            output_file << num << " "; 
        }

        std::cout << "Success! Created 'output.txt' and 'secret_key.txt'\n";
        output_file.close();
    }
    else if (mode == "decrypt")
    {
        if (argc < 4) 
        {
            std::cout << "Error: Missing arguments.\nUsage: " << argv[0] << " decrypt <secret_key.txt> <output.txt>\n";
            return 1;
        }

        std::string key_file_path = argv[2];
        std::string encrypted_file_path = argv[3];

        std::string decrypted = decrypt(key_file_path, encrypted_file_path);
        std::cout << "Decrypted text:\n" << decrypted << "\n";
    }
    else
    {
        std::cout << "Invalid mode. Use 'encrypt' or 'decrypt'.\n";
        return 1;
    }

    return 0;
}