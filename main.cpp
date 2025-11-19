#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<int> encrypt(std::string file_path)
{
    std::ifstream file;
    file.open(file_path);
    if (!file)
    {
        std::cout << "Unable to open file\n";
        std::exit(1);
    }

    std::string plain_text;
    std::string word;
    while (file >> word)
    {
        plain_text += word;
        plain_text += " ";
    }
    file.close();

    std::vector<int> key;
    for (std::size_t i = 0; i < plain_text.length(); i++)
    {
        int rand_num = rand() % 256;
        key.push_back(rand_num);
    }

    std::ofstream key_file("secret_key.txt");
    for (int key_value : key)
    {
        key_file << key_value << " ";
    }
    key_file.close();

    std::vector<int> encrypted;
    for (std::size_t i = 0; i < plain_text.length(); i++)
    {
        int xored = plain_text[i] ^ key[i];
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