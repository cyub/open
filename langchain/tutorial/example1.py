from langchain.chat_models import init_chat_model
import os

api_key = os.environ["SILICONFLOW_API_KEY"]

model = init_chat_model(
    model="Qwen/Qwen3-8B",
    model_provider="openai",
    base_url="https://api.siliconflow.cn/v1/",  # 硅基流动模型的请求url
    api_key=api_key,
)

question = "你好，请介绍一下自己"
result = model.invoke(question)
print(result)
