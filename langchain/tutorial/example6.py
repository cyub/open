from langchain.chat_models import init_chat_model

from langchain_core.output_parsers import JsonOutputParser
from pydantic import BaseModel, Field
from langchain_core.prompts import PromptTemplate
import os

api_key = os.environ["SILICONFLOW_API_KEY"]
model = init_chat_model(
    model="Qwen/Qwen3-8B",
    model_provider="openai",
    base_url="https://api.siliconflow.cn/v1/",  # 硅基流动模型的请求url
    api_key=api_key,
)


class Person(BaseModel):
    name: str = Field(..., description="用户姓名")
    age: int = Field(..., description="用户年龄")


parser = JsonOutputParser(pydantic_object=Person)
prompt = PromptTemplate(
    template="请根据以下内容提取用户信息，并返回 JSON 格式：\n{input}\n\n{format_instructions}",
    input_variables=["query"],
    partial_variables={"format_instructions": parser.get_format_instructions()},
)
chain = prompt | model | parser

result = chain.invoke({"input": "用户叫李雷，今年25岁，是一名工程师。"})

print(result)
