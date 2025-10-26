from langchain.chat_models import init_chat_model
import os

api_key = os.environ["DEEPSEEK_API_KEY"]

model = init_chat_model(
    model="deepseek-reasoner",  # deepseek-chat表示调用DeepSeek-v3模型，deepseek-reasoner表示调用DeepSeek-R1模型，
    model_provider="deepseek",  # 模型提供商写deepseek
    api_key=api_key,
)

question = "你好，请介绍一下自己"

result = model.invoke(question)

print(result)
